#version 330

in vec2 uv;
out vec3 color;
const float PI = 3.141592;

uniform sampler1D colormap;

void main() {

    float v = sin(uv.x*10*PI); // 2pi*5 = 5 periods
    float w = sin(uv.y*10*PI);
    float value = (v*w) + 0.5; // +0.5 : to make the green squares bigger

    color = texture(colormap, value).rgb;
}


