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

uniform int upper;

uniform float offsetX;
uniform float offsetY;

// lanscape values
uniform float lake_level;
uniform float height_scale;

in vec2 uv;
in vec4 vpoint_mv;
in mat4 MV;

in float transparency;

out vec4 color;

vec3 color1 = vec3(0,0,0);


void choosecolor1OnHeight(float height, vec3 normal_mv) {

    float sand_level = lake_level + 0.02*height_scale;
    float grass_level = sand_level + 0.15*height_scale;
    float mountains_level = grass_level + 0.2*height_scale;

    // Initalize the texture vectors and the blending factor
    vec4 t1 = vec4(0.0);
    vec4 t2 = vec4(0.0);
    float blend_factor = 0.0;
    float scale_factor = 18; // used to make textures smaller so we can repeat them

    vec2 uv_offset_scale = (uv + (vec2(offsetX, offsetY)+1)/1.3f)*scale_factor;

    if(height < lake_level) {
        // Compute the blend factor which depends on the height
        blend_factor = (height - lake_level)/(sand_level - lake_level);

        t1 = 0.85*texture(tex_sand, uv_offset_scale);
        t2 = texture(tex_sand, uv_offset_scale);
        color1 = mix(t1, t2, blend_factor).xyz;
    } else if(height <= sand_level) {
        // Sand & Lake level
        color1 = texture(tex_sand, uv_offset_scale).xyz;
    } else if(height <= grass_level) {
        // Grass & Forest level
        t1 = texture(tex_sand, uv_offset_scale);
        t2 = texture(tex_grass, uv_offset_scale);

        // Compute the blend factor which depends on the height
        blend_factor = (height - sand_level)/(grass_level - sand_level);

        if(normal_mv.x < 0.2) {
            color1 = t1.xyz;
        } else {
            color1 = t2.xyz;
        }

        // Equivalent to color1 = (t1*(1-grass_height) + t2*grass_height).xyz;
        color1 = mix(t1, t2, blend_factor).xyz;
    } else if(height <= mountains_level) {
        // Rock & Mountains Level
        t1 = texture(tex_grass, uv_offset_scale);
        t2 = texture(tex_rock, uv_offset_scale);

        // Compute the blend factor which depends on the height
        blend_factor = (height - grass_level)/(mountains_level - grass_level);

        color1 = mix(t1, t2, blend_factor).xyz;
    } else {
        // Snow of moutains level
        t1 = texture(tex_rock, uv_offset_scale);
        t2 = texture(tex_snow, uv_offset_scale);

        // Compute the blend factor which depends on the height
        float snow_height = (height - mountains_level)/(1.0 - mountains_level);

        color1 = mix(t1, t2, snow_height).xyz;
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


    choosecolor1OnHeight(height, normal_mv);



    // PHONG SHADING
    //color1=vec3(0);

    vec3 light_dir = light_pos - vpoint_mv.xyz;
    vec3 view_dir = -vpoint_mv.xyz;

    // ambient term
    vec3 ambient = ka * La;
    //color1 += ambient;

    vec3 n = normalize(normal_mv);
    vec3 l = normalize(light_dir);
    float lambert = dot(n,l);

    if(lambert > 0.0) {

        // diffuse term
        vec3 diffuse = Ld*kd*lambert;
        color1 += diffuse;

        // specular term
        vec3 v = normalize(view_dir);
        vec3 reflect = reflect(-l, n);
        vec3 specular = Ls*ks*pow(max(dot(reflect,v), 0.0), alpha);
        //color1 += specular;
    }
    //transparency for direant pixels
    color=vec4(color1, transparency);
}
