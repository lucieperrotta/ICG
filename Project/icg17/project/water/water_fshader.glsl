#version 330
uniform sampler2D tex;

in vec2 uv;

out vec3 color;

void main() {

    /// TODO: query window_width/height using the textureSize(..) function on tex_mirror
    ivec2 tex_size = textureSize(tex,0);
    float window_width = float(tex_size.x);
    float window_height = float(tex_size.y);

    /// TODO: use gl_FragCoord to build a new [_u,_v] coordinate to query the framebuffer
    /// NOTE: make sure you normalize gl_FragCoord by window_width/height
    float _u = gl_FragCoord.x/window_width;
    float _v = 1 - gl_FragCoord.y/window_height;

    /// NOTE: you will have to flip the "v" coordinate as framebuffer is upside/down
    vec3 blue = vec3(0.1,0.3,0.8);
    color = mix(vec3(0,0,1), texture(tex,vec2(_u,_v)).rgb, vec3(0.3));

}
