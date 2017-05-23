#version 330
uniform sampler2D tex_water;
uniform float lake_level;

uniform float time;

uniform float offsetX;
uniform float offsetY;

uniform float water_width;
uniform float water_height;

in vec2 uv;

in float transparency;

out vec4 color;

void main() {

    ivec2 tex_size = textureSize(tex_water,0);
    float window_width = float(tex_size.x);
    float window_height = float(tex_size.y);

    float _u = gl_FragCoord.x/window_width;
    float _v = gl_FragCoord.y/window_height;

    // wave effect
    const float PI = 3.1415;
    float v = 7*PI;
    float acc = 2.;
    float amplitude = 0.3;
    float wave = (sin((uv.x*v-time)*acc)+cos((uv.y*v-time)*acc))*amplitude + 1.5;
    vec4 wave_color = vec4(wave);


    //vec4 blue = vec4(27./255., 41./255., 92./255., 0.8);
    vec4 blue = vec4(0./255., 124./255., 195./255., 1);
    vec4 tex_water_color = texture(tex_water,vec2(_u,1-_v)).rgba;
    vec4 water_blue_color = mix(blue,tex_water_color,vec4(0.4));
    vec3 final_color = mix(wave_color,water_blue_color,vec4(0.9)).rgb;

    // mix hardcoded with skybox color to et smoother fade away
    final_color = mix(final_color, vec3(110./255., 110./255., 120./255.), pow(1-transparency, 0.5));

    color=vec4(final_color, 0.5*transparency);
}
