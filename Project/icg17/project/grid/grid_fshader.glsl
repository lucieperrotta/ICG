#version 330
uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;
uniform vec3 light_pos;

uniform float time;

uniform sampler1D tex_coloring;
uniform sampler2D tex;

uniform sampler2D tex_sand;
uniform sampler2D tex_grass;
uniform sampler2D tex_rock;
uniform sampler2D tex_snow;

uniform float lake_level;

in float lake_height;
in vec2 uv;
in vec4 vpoint_mv;
in mat4 MV;

out vec3 color;

void main() {
    // height normalization from 0 to 1 (lake_height is from 0.55 to 0.85)
    float norm_height = (lake_height - lake_level)/(0.85 - lake_level);

    // initialize height limits (lake, forest and mountains)
    // the values can be changed here
    float sand_level = 0.;
    float grass_level = 0.4;
    float mountains_level = 0.6;

    // Initalize the texture vectors and the blending factor
    vec4 t1 = vec4(0.0);
    vec4 t2 = vec4(0.0);
    float blend_factor = 0.0;

    if(norm_height <= sand_level) {
        // Sand & Lake level
        /*t1 = texture(tex_sand, uv);
        t2 = texture(tex_grass, uv);

        // Compute the blend factor which depends on the height
        blend_factor = (norm_height - sand_level)/(grass_level - sand_level);

        // Equivalent to color = (t1*(1-grass_height) + t2*grass_height).xyz;
        color = mix(t1, t2, blend_factor).xyz;*/
        //printf(norm_height);
        color = texture(tex_sand, uv).xyz;
    } else if(norm_height <= grass_level) {
        // Grass & Forest level
        t1 = texture(tex_sand, uv);
        t2 = texture(tex_grass, uv);

        // Compute the blend factor which depends on the height
        blend_factor = (norm_height - sand_level)/(grass_level - sand_level);

        // Equivalent to color = (t1*(1-grass_height) + t2*grass_height).xyz;
        color = mix(t1, t2, blend_factor).xyz;
    } else if(norm_height <= mountains_level) {
        // Rock & Mountains Level
        t1 = texture(tex_grass, uv);
        t2 = texture(tex_rock, uv);

        // Compute
        blend_factor = (norm_height - grass_level)/(mountains_level - grass_level);

        color = mix(t1, t2, blend_factor).xyz;
    } else {
        // Snow of moutains level
        t1 = texture(tex_rock, uv);
        t2 = texture(tex_snow, uv);

        // Compute
        float snow_height = (norm_height - mountains_level)/(1.0 - mountains_level);

        color = mix(t1, t2, snow_height).xyz;
    }

    //color = mix();


    // Forest & snow level
    //color = texture(tex_grass, uv).xyz; // CHANGED
    //color = texture(tex_coloring, norm_height).rgb;



    // PHONG SHADING
    //color=vec3(0);

    // compute normal : compute
    float delta = 0.09; // how much we want to be precize : distance between 2 points we take to compute gradient
    // vectors of difference of elevation (r coordinate of texture) between 2 points
    float xz = texture(tex, uv + vec2(delta/2.0, 0)).r - texture(tex, uv - vec2(delta/2.0, 0)).r;
    float yz = texture(tex, uv + vec2(0, delta/2.0)).r - texture(tex, uv - vec2(0, delta/2.0)).r;
    vec3 x = vec3(delta, 0, xz);
    vec3 y = vec3(0, delta, yz);
    vec3 normal = normalize(cross(x,y));

    vec3 normal_mv = vec3(MV * vec4(normal, 0));
    vec3 light_dir = light_pos - vpoint_mv.xyz;
    vec3 view_dir = -vpoint_mv.xyz;

    // ambient term
    vec3 ambient = ka * La;
    color += ambient;



    vec3 n = normalize(normal_mv);
    vec3 l = normalize(light_dir);
    float lambert = dot(n,l);

    if(lambert > 0.0) {

        // diffuse term
        vec3 diffuse = Ld*kd*lambert;
        color += diffuse;

        // specular term
        vec3 v = normalize(view_dir);
        vec3 reflect = reflect(-l, n);
        vec3 specular = Ls*ks*pow(max(dot(reflect,v), 0.0), alpha);
        //color += specular;
    }
}
