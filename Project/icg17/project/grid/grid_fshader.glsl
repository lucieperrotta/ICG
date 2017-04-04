#version 330

in vec2 uv;

out vec3 color;
uniform float time;
uniform sampler2D tex;

void main() {
    const float PI = 3.1415;
    float v = 0.7*PI;
    float acc = 2;
    float amplitude = 0.1;
    float height = (sin((uv.x*v-time)*acc)+cos((uv.y*v-time)*acc))*amplitude;
    color = vec3(69./255. + height*120./255, 184./255. + (height)*120/255, 255./255.);
}
