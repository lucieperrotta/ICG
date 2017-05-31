#version 330
uniform sampler2D tex_water;
uniform sampler2D tex_dudv;
uniform sampler2D tex_lower_grid;
uniform float time;

in vec2 uv;
in float transparency;
in vec2 textureCoords;

out vec4 color;

void main() {

    ivec2 tex_size = textureSize(tex_water,0);
    float window_width = float(tex_size.x);
    float window_height = float(tex_size.y);

    float _u = gl_FragCoord.x/window_width;
    float _v = gl_FragCoord.y/window_height;

    // wave effect color
    const float PI = 3.1415;
    float v = 5*PI;
    float acc = 2.;
    float amplitude = 0.3;
    float wave = (sin((uv.x*v-time)*acc)+cos((uv.y*v-time)*acc))*amplitude + 1.5;
    vec4 wave_color = vec4(wave);
    vec4 blue = vec4(15./255., 40./255., 150./255., 1.0);
    float strength = 0.02;

    vec2 distorsion = ((texture(tex_dudv, textureCoords).rg)*2.0 - 1.0)*vec2(strength,strength);
    vec2 refractCoord = clamp(vec2(_u + distorsion.x,_v + distorsion.y),0.01,0.999); // clamp to fit screen
    vec2 reflectCoord = clamp(vec2(_u + distorsion.x,1-_v + distorsion.y),0.01,0.999); // clamp to fit screen

    // Reflection
    vec4 reflect_color = texture(tex_water, reflectCoord); // 1-v to inverse texture
    // refraction
    vec4 refract_color = texture(tex_lower_grid, refractCoord);

    vec4 color1 = mix(blue, refract_color, 1);
    vec4 reflect_intensity = vec4(0.50);
    vec4 water_blue_color = mix(color1, reflect_color, reflect_intensity);


    // little adjustments
    water_blue_color -= 0.20;
    water_blue_color.b += 15/255.;
    vec3 final_color = mix(wave_color, water_blue_color, vec4(0.9)).rgb;

    // mix hardcoded with skybox color to et smoother fade away
    final_color = mix(final_color, vec3(100./255., 100./255., 200./255.), pow(1-transparency, 1.0));

    color = vec4(final_color, 0.7*transparency);
}
