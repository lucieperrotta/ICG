#version 330

in vec3 light_dir;
in vec3 spot_dir;
in vec3 view_dir;
in mat4 MV;
in vec3 normal_mv;

out vec3 color;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

const float spot_cos_cutoff = 0.996; // we increased this value
const float spot_exp = 350; // we increased this value

void main() {    

    // Normalizations
    vec3 n = normalize(normal_mv);
    vec3 l = normalize(light_dir);
    vec3 v = normalize(view_dir);
    vec3 s = normalize(spot_dir);

    // ambient term
    vec3 ambient = ka * La;
    color += ambient;

    float spotEffect = dot(-l,s);

    if( spotEffect > spot_cos_cutoff){

        float lambert = dot(n,l);

        if(lambert > 0.0) {

            // diffuse term
            vec3 diffuse = Ld*kd*lambert;
            color += diffuse;

            // specular term
            vec3 reflect = reflect(-l, n);
            vec3 specular = Ls*ks*pow(max(dot(reflect,v), 0.0), alpha);
            color += specular;
        }

        // Actual spotlight
        spotEffect = pow(spotEffect, spot_exp);
        color *= spotEffect;
    }
}
