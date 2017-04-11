#version 330
uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;
uniform vec3 light_pos;
uniform sampler2D tex;
uniform float time;

in float lake_height;
in vec2 uv;
in vec4 vpoint_mv;
in mat4 MV;

out vec3 color;

void main() {
    // height normalization from 0 to 1 (lake_height is from 0.55 to 0.85)
    float norm_height = (lake_height-0.7)*14;

    // initialize RGB colors
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;

    // initialize height limits (lake, forest and mountains)
    // the values can be changed here
    float lake_level = 0.7;
    //float forest_level = 0.75;
    float mountains_level = 0.8;

    if(lake_height <= lake_level) {
        // Lake level
        color= vec3(23./255., 10./255., 200./255.);
   /* } else if(lake_height <= forest_level) {
        // Sand level
        red = 0.2;
        green = 0.3;
        blue = 0.07;

        float forest_height = norm_height - forest_level - lake_level;

        color = (1-forest_height) * vec3(red, green, blue)
                + (forest_height) * vec3(0.15, green, blue);*/
    } else if (lake_height <= mountains_level) {
        // Forest color
        red = 0.09;
        green = 0.25;
        blue = 0.01;

        float forest_height = (norm_height - lake_level);

        color = (forest_height) * vec3(0.0, 0.10, 0.0)
              + (1-forest_height) * vec3(red, green, blue);
    } else {
        // Snow level
        float mountains_height = norm_height - mountains_level;
        color = (mountains_height) * vec3(0.2, 0.2, 0.2)
              + (1 - mountains_height) * vec3(0.0, 0.0, 0.0);
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
