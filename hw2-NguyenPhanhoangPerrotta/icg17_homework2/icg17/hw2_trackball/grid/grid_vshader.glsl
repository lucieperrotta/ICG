#version 330

in vec2 position;

out vec2 uv;

uniform mat4 MVP;
uniform float time;

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    // convert the 2D position into 3D positions that all lay in a horizontal
    // plane.
    // TODO 6: animate the height of the grid points as a sine function of the
    // 'time' and the position ('uv') within the grid.
    const float PI = 3.1415;
    float v = 0.7*PI;
    float acc = 2.;
    float amplitude = 0.1;
    float height = (sin((uv.x*v-time)*acc)+cos((uv.y*v-time)*acc))*amplitude;
    vec3 pos_3d = vec3(position.x, height, -position.y);

    gl_Position = MVP * vec4(pos_3d, 1.0);
}
