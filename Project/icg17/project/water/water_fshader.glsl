#version 330
uniform sampler2D tex_water;

in vec2 uv;

out vec4 color;

void main() {

    /// TODO: query window_width/height using the textureSize(..) function on tex_mirror
    ivec2 tex_size = textureSize(tex_water,0);
    float window_width = float(tex_size.x);
    float window_height = float(tex_size.y);

    /// TODO: use gl_FragCoord to build a new [_u,_v] coordinate to query the framebuffer
    /// NOTE: make sure you normalize gl_FragCoord by window_width/height
    float _u = gl_FragCoord.x/window_width;
    float _v = 1 - gl_FragCoord.y/window_height;

    /// NOTE: you will have to flip the "v" coordinate as framebuffer is upside/down
    vec4 blue = vec4(27./255., 41./255., 92./255., 0.8);
    //color = mix(blue, texture(tex,vec2(_u,_v)).rrra, vec4(0.1));
    color = texture(tex_water,vec2(_u,_v));
}
