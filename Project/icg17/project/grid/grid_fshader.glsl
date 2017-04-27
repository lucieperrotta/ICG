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

in float lake_height;
in vec2 uv;
in vec4 vpoint_mv;
in mat4 MV;

out vec3 color;

void main() {
    // height normalization from 0 to 1 (lake_height is from 0.55 to 0.85)
    float norm_height = (lake_height - 0.71)/0.6;

    // initialize RGB colors
    vec3 yellow = vec3(1.0, 1.0, 0.0);
    vec3 green = vec3(0.0, 1.0, 0.0);
    vec3 grey = vec3(0.0);

    mat3 table = mat3(yellow, green, grey);

    // initialize height limits (lake, forest and mountains)
    // the values can be changed here
    float lake_level = 0.7;
    //float forest_level = 0.75;
    float mountains_level = 0.8;

    if(lake_height <= lake_level) {
        // Lake level
        color = vec3(0./255., 90/255., 170./255.);
    } else {
        // Forest & snow level
       color = texture(tex_grass, uv).xyz;
        //color = texture(tex_coloring, norm_height).rgb;
    }

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
