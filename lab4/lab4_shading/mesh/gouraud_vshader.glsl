#version 330

in vec3 vpoint;
in vec3 vnormal;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

out vec3 vcolor;

void main() {
    mat4 MV = view * model;
    vec4 vpoint_mv = MV * vec4(vpoint, 1.0);
    gl_Position = projection * vpoint_mv;
    //>>>>>>>>>> TODO >>>>>>>>>>>
    // TODO 0.1: Gouraud shading.
    // 1) compute the normal using the model_view matrix.
    vec3 normal_mv = normalize(MV * vec4(vnormal,0.f)).xyz;
    // 2) compute the light direction light_dir.
    vec3 light_dir = normalize(vec4(light_pos,0.f) - vec4(MV)).xyz;
    // 3) compute the view direction view_dir.
    vec3 view_dir = normalize(-vpoint_mv.xyz);
    // 4) compute per vertex color

    vec3 reflect = reflect(-light_dir, normal_mv);

    float rv = dot(reflect,view_dir);
    float nl = dot(normal_mv,light_dir);

    // clamp negative values to 0
    if(nl < 0) {nl = 0;}
    if(rv < 0) {rv = 0;}

    vec3 ambient = ka * La;
    vec3 diffuse = kd * nl * Ld;
    vec3 specular = ks *(pow(rv,alpha))* Ls;

    vcolor = ambient + diffuse + specular;

}
