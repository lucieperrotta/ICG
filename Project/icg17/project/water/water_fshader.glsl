#version 330
uniform sampler2D tex_water;

in vec2 uv;

out vec4 color;

void main() {

    ivec2 tex_size = textureSize(tex_water,0);
    float window_width = float(tex_size.x);
    float window_height = float(tex_size.y);

    float _u = gl_FragCoord.x/window_width;
    float _v = gl_FragCoord.y/window_height;

    vec4 blue = vec4(27./255., 41./255., 92./255., 0.8);
    color = vec4(mix(blue,texture(tex_water,vec2(_u,1-_v)).rrra,vec4(0.3)).rgb,0.6);
}
