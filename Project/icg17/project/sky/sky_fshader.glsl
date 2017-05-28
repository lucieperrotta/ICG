#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex_sky;
uniform sampler2D tex_sky_night;

uniform float time;

void main() {
    vec3 tex_day = texture(tex_sky, uv).rgb;
    vec3 tex_night = texture(tex_sky_night, uv).rgb;
    float opacity = time/40;
    color = mix(tex_day, tex_night, opacity);
}
