#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex;
uniform float tex_width;
uniform float tex_height;

uniform int k_length;
uniform float kernel[450];

// = 0 -> x
// = 1 -> y
uniform int axis;

void main() {

    // gaussian convolution
    vec3 color_tot = vec3(0.0);
    float weight_tot = 0;
    vec3 neigh_color;
    float middle = k_length/2.0f;
    for(int i = 0; i < k_length; i++){

        // check if you have to compute on the x or y axis
        if(axis==1) {
            neigh_color = texture(tex, uv+vec2(0,(i-middle)/tex_height)).rgb;
        }
        else {
            neigh_color = texture(tex, uv+vec2((i-middle)/tex_width,0)).rgb;
        }

        color_tot += kernel[i] * neigh_color;
        weight_tot += kernel[i];
    }
    color = color_tot / weight_tot; // ensure \int w = 1

}

