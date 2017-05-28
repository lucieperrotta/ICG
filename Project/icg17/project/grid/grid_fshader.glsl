#version 330
// values for brightness
uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;
uniform vec3 light_pos;

uniform float time;

// textures
uniform sampler2D tex_grid;
uniform sampler2D tex_sand;
uniform sampler2D tex_grass;
uniform sampler2D tex_rock;
uniform sampler2D tex_snow;
uniform sampler2D tex_lol;

uniform int upper;

uniform float offsetX;
uniform float offsetY;

// lanscape values
// float lake_level = 0.25f;
// float height_scale = 0.7;
uniform float lake_level;
uniform float height_scale;

in vec2 uv;
in vec4 vpoint_mv;
in mat4 MV;

in float transparency;

out vec4 color;

vec3 raw_color = vec3(0,0,0);

void chooseColorOnHeight(float height, vec3 normal) {

    float sand_level = lake_level + 0.04*height;
    float sand_grass_transition = sand_level + 0.1*height;
    float grass_level = sand_grass_transition + 0.05*height;
    float grass_rock_transition = grass_level + 0.1*height;
    float mountains_level = grass_rock_transition + 0.05*height;
    float rock_mountains_transition = mountains_level + 0.03*height;

    // Initalize the texture vectors and the blending factor
    vec4 t1 = vec4(0.0);
    vec4 t2 = vec4(0.0);
    float blend_factor = 0.0; // defines the area of the texture
    float scale_factor = 25; // used to make textures smaller so we can repeat them

    vec2 uv_offset_scale = (uv + (vec2(offsetX, offsetY)+1)/1.3f)*scale_factor;

    if(height < lake_level) {
        // Lake level (The sand under the water is a bit dark)

        // Compute the blend factor which depends on the height
        blend_factor = (height - lake_level)/(sand_level - lake_level);

        t1 = 0.9*texture(tex_sand, uv_offset_scale);
        t2 = texture(tex_sand, uv_offset_scale);

        raw_color = mix(t1, t2, blend_factor).xyz;
    } else if(height <= sand_level) {
        // Sand level
        raw_color = texture(tex_sand, uv_offset_scale).xyz;
    } else if(height <= sand_grass_transition) {
        // Sand & Grass level
        t1 = texture(tex_sand, uv_offset_scale);
        t2 = texture(tex_grass, uv_offset_scale);

        // Compute the blend factor which depends on the height
        blend_factor = (height - sand_level)/(sand_grass_transition - sand_level);

        raw_color = mix(t1, t2, blend_factor).xyz;
    } else if(height <= grass_level) {
        // Grass & Forest level
        raw_color = texture(tex_grass, uv_offset_scale).xyz;
    } else if(height <= grass_rock_transition) {
        // Grass & Rock level
        t1 = texture(tex_grass, uv_offset_scale);
        t2 = texture(tex_rock, uv_offset_scale);

        blend_factor = (height - grass_level)/(grass_rock_transition - grass_level);

        raw_color = mix(t1, t2, blend_factor).xyz;
    } else if(height <= mountains_level) {
        // Rock & Mountains Level
        raw_color = texture(tex_rock, uv_offset_scale).xyz;
    } else /*if(height <= rock_mountains_transition)*/ {
        // Rock & Snow level
        t1 = texture(tex_rock, uv_offset_scale);
        t2 = texture(tex_snow, uv_offset_scale);

        blend_factor = 0.7*(height - mountains_level)/(rock_mountains_transition - mountains_level);

        float f = 10;
        float amplitude = 230;
        float sin_value = rock_mountains_transition
                + sin((uv_offset_scale.y+uv_offset_scale.x)*f)/amplitude
                 + sin((uv_offset_scale.y+uv_offset_scale.x)*3)/amplitude;

        raw_color = (sin_value > height) ? mix(t1, t2, blend_factor).xyz : t2.xyz;
    }
}

void main() {

    float height = texture(tex_grid, uv).x;
    if(
            (((upper==0)) && (height >= lake_level)) ||
            ((upper==1)) && (height < lake_level)
            ) discard;

    // compute normal : compute
    float delta = 0.09; // how much we want to be precize : distance between 2 points we take to compute gradient
    // vectors of difference of elevation (r coordinate of texture) between 2 points
    float xz = texture(tex_grid, uv + vec2(delta/2.0, 0)).r - texture(tex_grid, uv - vec2(delta/2.0, 0)).r;
    float yz = texture(tex_grid, uv + vec2(0, delta/2.0)).r - texture(tex_grid, uv - vec2(0, delta/2.0)).r;
    vec3 x = vec3(delta, 0, xz);
    vec3 y = vec3(0, delta, yz);
    vec3 normal = normalize(cross(x,y));
    vec3 normal_mv = vec3(MV * vec4(normal, 0));

    chooseColorOnHeight(height, normal);

    // PHONG SHADING
    //raw_color=vec3(0);

    vec3 light_dir = light_pos - vpoint_mv.xyz;
    vec3 view_dir = -vpoint_mv.xyz;

    // ambient term
    vec3 ambient = ka * La;
    //raw_color += ambient;

    vec3 n = normalize(normal_mv);
    vec3 l = normalize(light_dir);
    float lambert = dot(n,l);

    if(lambert > 0.0) {

        // diffuse term
        vec3 diffuse = Ld*kd*lambert*0.5;
        raw_color += diffuse;

        // specular term
        vec3 v = normalize(view_dir);
        vec3 reflect = reflect(-l, n);
        vec3 specular = Ls*ks*pow(max(dot(reflect, v), 0.0), alpha);
        // raw_color += specular;
    }
    // transparency for direant pixels
    raw_color.r += 0.07;
    raw_color.g += 0.02;
    color = vec4(raw_color, transparency);
}
