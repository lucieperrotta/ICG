#version 330
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform float time;
uniform sampler2D tex_water;
uniform float lake_level;

in vec2 position;

out float lake_height;
out vec4 vpoint_mv;
out mat4 MV;
out vec2 uv;


void main() {
    // (position + vec2(1.0, 1.0)) * 0.5;
    uv = position;

    float height = lake_level;

    // model view matrix -> used for perspective and stuff like this
    MV = view * model;

    vec3 pos_3d = vec3(position.x, height, -position.y);
    gl_Position =  projection*MV * vec4(pos_3d, 1.0);

}
