#version 330

in vec3 vpoint;
in vec3 vnormal;


uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;
//uniform vec3 spot_dir;

out vec3 normal_mv;
out vec3 light_dir;
out vec3 view_dir;
out vec3 spot_dir;

void main() {
    mat4 MV = view * model;
    vec4 vpoint_mv = MV * vec4(vpoint, 1.0);
    gl_Position = projection * vpoint_mv;

    // see this explanation to understand why we do the transpose of the inverse :
    // http://www.lighthouse3d.com/tutorials/glsl-12-tutorial/the-normal-matrix/
    normal_mv = normalize(mat3(transpose(inverse(MV))) * vnormal);
    light_dir = light_pos - vpoint_mv.xyz;
    view_dir = -vpoint_mv.xyz;
    spot_dir = -view_dir;
}
