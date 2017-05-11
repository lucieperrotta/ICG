#version 330
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform float lake_level;

uniform float time;

uniform sampler2D tex_grid;

uniform float height_scale;

in vec2 position;

out vec4 vpoint_mv;
out mat4 MV;
out vec2 uv;


void main() {

    uv = (position + vec2(1.0, 1.0)) * 0.5;

    // model view matrix -> used for perspective and stuff like this
    MV = view * model;

    // use the texture to obtain z axis
    float height = texture(tex_grid, uv).x;

    // draw above lake if upper == 1
    // draw below lake if upper == 0

    float height_scale = 1.8;


    vec3 pos_3d = vec3(position.x, height/height_scale, -position.y);
    gl_Position =  projection*MV * vec4(pos_3d, 1.0);

    // point position to send to fshader for shading
    vpoint_mv = MV * vec4(pos_3d, 1.0);


}
