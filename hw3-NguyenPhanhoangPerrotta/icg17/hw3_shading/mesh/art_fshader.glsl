#version 330

out vec3 color;

in vec3 light_dir;
in vec3 view_dir;
in mat4 MV;
in vec3 normal_mv;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;

uniform float alpha;
uniform sampler2D tex2D;

void main() {
    color = vec3(0.0, 0.0, 0.0);

    //>>>>>>>>>> TODO >>>>>>>>>>>
    // TODO 3.2: Artistic shading.
    // 1) compute the output color by doing a look-up in the texture using the
    //    texture sampler tex.
    //<<<<<<<<<< TODO <<<<<<<<<<<
    vec3 n = normalize(normal_mv);
    vec3 l = normalize(light_dir);
    float lambert = dot(n,l);

    // specular term
    vec3 v = normalize(view_dir);
    vec3 reflect = reflect(-l, n);

    /// 1) compute ambient term.
    vec3 ambient = ka * La * lambert;
    //color += ambient;

    if(lambert > 0.0) {

        vec3 color_factor = texture(tex2D, vec2(lambert, pow(max(dot(reflect,v), 0.0), alpha))).xyz;
    /// 2) compute diffuse term using the texture sampler tex.
        vec3 diffuse = Ld*kd*color_factor;
        vec3 specular = Ls*ks*color_factor;
        color += color_factor.rgb;
    }


}
