#version 330

out vec3 color;

in vec3 light_dir;
in vec3 view_dir;
in mat4 MV;
in vec3 normal_mv;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

uniform sampler1D tex1D;

void main() {

    color = vec3(0.0,0.0,0.0);

    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO 2.2: Toon shading.
    vec3 n = normalize(normal_mv);
    vec3 l = normalize(light_dir);
    float lambert = dot(n,l);

    /// 1) compute ambient term.
    vec3 ambient = ka * La;
    color += ambient;

    if(lambert > 0.0) {
    /// 2) compute diffuse term using the texture sampler tex.
        vec3 diffuse = Ld*kd*texture(tex1D, lambert).x;
        color += diffuse;
    /// 3) compute specular term using the texture sampler tex.

        // specular term
        vec3 v = normalize(view_dir);
        vec3 reflect = reflect(-l, n);
        /*float reflectCartoon = texture(tex1D, dot(reflect,v));*/
        vec3 specular = Ls*ks*pow(max(dot(reflect,v), 0.0), alpha);
        color += specular;
    }
    ///<<<<<<<<<< TODO <<<<<<<<<<<
}
