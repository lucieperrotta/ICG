#version 330
out vec3 color;

in vec3 light_dir;
in vec3 view_dir;
in mat4 MV;
in vec3 normal_mv;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

void main() {

    vec3 ambient = ka * La;
    color += ambient;

    vec3 n = normalize(normal_mv);
    vec3 l = normalize(light_dir);
    float lambert = dot(n,l);

    if(lambert > 0.0) {
        vec3 diffuse = Ld*kd*lambert;
        color += diffuse;

        vec3 v = normalize(view_dir);
        vec3 reflect = reflect(-l, n);
        vec3 specular = Ls*ks*pow(max(dot(reflect,v), 0.0), alpha);
        color += specular;
    }

}
