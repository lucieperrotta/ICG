#version 330
uniform sampler2D tex_water;
uniform float lake_level;

uniform float time;

in vec2 uv;

in float transparency;

out vec4 color;

void main() {

    ivec2 tex_size = textureSize(tex_water,0);
    float window_width = float(tex_size.x);
    float window_height = float(tex_size.y);

    float _u = gl_FragCoord.x/window_width;
    float _v = gl_FragCoord.y/window_height;

    // wave effect color
    const float PI = 3.1415;
    float v = 10*PI;
    float acc = 2.;
    float amplitude = 0.3;
    float wave = (sin((uv.x*v-time)*acc)+cos((uv.y*v-time)*acc))*amplitude + 1.5;
    vec4 wave_color = vec4(wave);

    vec4 blue = vec4(20./255., 50./255., 200./255., 1.0);
    vec4 tex_water_color = texture(tex_water,vec2(_u,1-_v)).rgba; // 1-v to inverse texture

    vec4 reflect_intensity = vec4(0.75);
    vec4 water_blue_color = mix(blue, tex_water_color, reflect_intensity);
    vec3 final_color = mix(wave_color, water_blue_color, vec4(0.9)).rgb;

    // mix hardcoded with skybox color to et smoother fade away
    final_color = mix(final_color, vec3(100./255., 100./255., 200./255.), pow(1-transparency, 1.0));

    color = vec4(final_color, 0.5*transparency);
}
