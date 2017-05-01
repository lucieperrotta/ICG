#version 330

uniform float delay_y;

in vec3 vpoint;
in vec2 vtexcoord;
out vec2 uv;


void main() {
    uv = vtexcoord;

    vec3 pos_3d = vpoint/3.5 - vec3(0.5,delay_y,0);
    gl_Position = vec4(pos_3d, 1.0);
}
