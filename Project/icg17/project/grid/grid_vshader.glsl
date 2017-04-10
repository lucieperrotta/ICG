#version 330
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

in vec2 position;

in vec3 vpoint;

out vec3 light_dir;
out vec3 view_dir;

out vec2 uv;

uniform float time;
uniform sampler2D tex;

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    // use the texture to obtain z axis
    //vec4 height = texture(tex, uv);
    vec4 height = vec4(light_pos, 0);

    /*const float PI = 3.1415;
    float v = 0.7*PI;
    float acc = 2;
    float amplitude = 0.1;
    float height = (sin((uv.x*v-time)*acc)+cos((uv.y*v-time)*acc))*amplitude;*/
    vec3 pos_3d = vec3(position.x, height.x, -position.y);

    gl_Position =  projection*view*model * vec4(pos_3d, 1.0);

}
