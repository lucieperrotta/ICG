#version 330
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform float lake_level;

uniform float time;

uniform sampler2D tex_grid;
uniform int upper;

in vec2 position;

out float lake_height;
out vec4 vpoint_mv;
out mat4 MV;
out vec2 uv;


void main() {

    uv = (position + vec2(1.0, 1.0)) * 0.5;

    // model view matrix -> used for perspective and stuff like this
    MV = view * model;

    // use the texture to obtain z axis
    vec4 height = texture(tex_grid, uv);
    // height goes from 0.55 -> 0.85

    // to have flat lakes
    lake_height = height.x;

    if(
            (((upper==1)) && (lake_height > lake_level)) ||
            ((upper==0)) && (lake_height <= lake_level)
            ){

        vec3 pos_3d = vec3(position.x, lake_height, -position.y);
        gl_Position =  projection*MV * vec4(pos_3d, 1.0);

        // point position to send to fshader for shading
        vpoint_mv = MV * vec4(pos_3d, 1.0);
    }

}
