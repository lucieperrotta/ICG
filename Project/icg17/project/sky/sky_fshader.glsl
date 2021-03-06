#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex_sky;

void main() {
    color = texture(tex_sky, uv).rgb;
    color -= 0.16;
    color.r += 0.02;
}
