#version 330

in vec2 position;

out vec2 uv;

uniform mat4 MVP;
uniform float time;
uniform sampler2D tex;

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    // use the texture to obtain z axis
    vec4 height = texture(tex, uv);
    vec3 pos_3d = vec3(position.x, height.x, -position.y);

    gl_Position = MVP * vec4(pos_3d, 1.0);


}
