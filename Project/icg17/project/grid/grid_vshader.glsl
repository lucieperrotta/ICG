#version 330
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform float time;
uniform sampler2D tex;

in vec2 position;
//in vec3 vpoint;

out vec4 vpoint_mv;

out vec2 uv;


void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    // use the texture to obtain z axis
    vec4 height = texture(tex, uv);
    //vec4 height = vec4(light_pos, 0);

    vec3 pos_3d = vec3(position.x, height.x, -position.y);
    gl_Position =  projection*view*model * vec4(pos_3d, 1.0);

    mat4 MV = view * model;
    vpoint_mv = MV * vec4(pos_3d, 1.0);
    //vpoint_mv = vec4(pos_3d,1.);



}
