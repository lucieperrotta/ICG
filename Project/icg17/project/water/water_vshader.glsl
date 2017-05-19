#version 330
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform float time;
uniform sampler2D tex_water;
uniform float lake_level;

uniform float cam_pos_x;
uniform float cam_pos_z;

in vec2 position;

out vec4 vpoint_mv;
out mat4 MV;
out vec2 uv;

out float transparency;


void main() {
    //uv = position;
    //float height = lake_level ;//+ sin(time*uv.x)/30;
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    // model view matrix -> used for perspective and stuff like this
    MV = view * model;

    const float PI = 3.1415;
    float v = 7*PI;
    float acc = 2.;
    float amplitude = 0.001;
    float height = (sin((uv.x*v-time)*acc)+cos((uv.y*v-time)*acc))*amplitude + lake_level;

    vec3 pos_3d = vec3(position.x, height, -position.y);
    gl_Position =  projection*MV * vec4(pos_3d, 1.0);

    // transparency for distant pixels (transparency gets stronger with quadratic order (1.5) )
    transparency = 1-pow(pow(pos_3d.x+cam_pos_x, 2) + pow(pos_3d.z+cam_pos_z, 2), 1.5);

}
