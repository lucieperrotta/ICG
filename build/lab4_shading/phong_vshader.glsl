#version 330
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

in vec3 vpoint;
in vec3 vnormal;

out vec3 normal_mv;
out vec3 light_dir;
out vec3 view_dir;
//out mat4 MV;

void main() {
    mat4 MV = view * model;
    vec4 vpoint_mv = MV * vec4(vpoint, 1.0);
    gl_Position = projection * vpoint_mv;

    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO 1.1: Phong shading.
    // 1) compute the normal using the model_view matrix.
    normal_mv = normalize((MV * vec4(vnormal,0.f)).xyz);
    // 2) compute the light direction light_dir.
    light_dir = normalize((vec4(light_pos,0.f) - vpoint_mv).xyz);
    // 3) compute the view direction view_dir.
    view_dir = normalize(-vpoint_mv.xyz);
}
