#version 330

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

out vec3 color;
in vec3 light_dir;
in vec3 view_dir;
in vec4 vpoint_mv;

void main() {

    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO 4.2: Flat shading.
    /// 1) compute triangle normal using dFdx and dFdy
    vec3 x = dFdx(vpoint_mv).xyz;
    vec3 y = dFdy(vpoint_mv).xyz;
    vec3 normal_mv = cross(x,y);

    /// 1) compute ambient term.
    color += La*ka;

    vec3 n = normalize(normal_mv);
    vec3 l = normalize(light_dir);

    float lambert = dot(n,l);
    if(lambert > 0.0) {
        /// 2) compute diffuse term.
        color += Ld*kd*lambert;

        /// 3) compute specular term.
        vec3 v = normalize(view_dir);
        vec3 r = reflect(-l,n);
        color += Ls*ks*pow(max(dot(r,v), 0.0), alpha);
    }
}
