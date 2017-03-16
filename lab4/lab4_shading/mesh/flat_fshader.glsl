#version 330

out vec3 color;

in vec4 vpoint_mv;
in vec3 light_dir, view_dir;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;


void main() {

    /// TODO 4.2: Flat shading.
    /// 1) compute triangle normal using dFdx and dFdy
    vec3 x = dFdx(vpoint_mv).xyz;
    vec3 y = dFdy(vpoint_mv).xyz;
    vec3 normal_mv = cross(x,y);

    vec3 reflect = reflect(-light_dir, normal_mv);

    float rv = dot(reflect,view_dir);
    float nl = dot(normal_mv,light_dir);

    // clamp negative values to 0
    if(nl < 0) {nl = 0;}
    if(rv < 0) {rv = 0;}

    // 1) compute ambient term.
    vec3 ambient = ka * La;
    // 2) compute diffuse term.
    vec3 diffuse = kd * nl * Ld;
    // 3) compute specular term.
    vec3 specular = ks *(pow(rv,alpha))* Ls;

    color = ambient + diffuse + specular;
}
