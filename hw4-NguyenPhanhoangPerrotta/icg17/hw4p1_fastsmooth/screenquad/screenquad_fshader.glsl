#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex;
uniform float tex_width;
uniform float tex_height;

uniform int kernel_size;
uniform float kernel[450];

uniform int axis;

void main() {

     // gaussian convolution
    float weight_tot = 0.0;
    float middle = kernel_size / 2.0;

    vec3 color_tot = vec3(0.0);
    vec3 neigh_color = vec3(0.0);

    for(int i = 0; i < kernel_size; i++){

       if(axis == 0) { // pass for the x axis
           neigh_color = texture(tex, uv + vec2((i - middle) / tex_width, 0.0)).rgb;
       } else {
           neigh_color = texture(tex, uv + vec2(0.0, (i - middle) / tex_height)).rgb;
       }


       color_tot += kernel[i] * neigh_color;
       weight_tot += kernel[i];
    }

    color = color_tot / weight_tot; // ensure \int w = 1

}

