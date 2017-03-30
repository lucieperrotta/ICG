#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex;
uniform float tex_width;
uniform float tex_height;

// Functions headers
vec2 hash(vec2 p);
float smooth_interpolation(float t);
float mix(float x, float y, float alpha);
float rand(vec2 c);
vec4 taylorInvSqrt(vec4 r);


void main() {

    // Grid size in squares
    float N = 16.;

    // Get to the correct square in grid (row number and column numer)
    vec2 colums_line_number = floor(uv*N);

    // Compute the pixel values [0, 1] of the 4 corners of the square
    vec2 bl = (colums_line_number+vec2(0.,0.) ) / N;
    vec2 br = (colums_line_number+vec2(0.,1.) ) / N;
    vec2 tl = (colums_line_number+vec2(1.,0.) ) / N;
    vec2 tr = (colums_line_number+vec2(1.,1.) ) / N;

    // Compute difference vectors from corners to actual point
    vec2 bl_diff = bl - uv; // a
    vec2 br_diff = br - uv; // b
    vec2 tl_diff = tl - uv; // c
    vec2 tr_diff = tr - uv; // d

    // Generate random vector for each corner
    vec2 bl_rand_vec = hash(bl); // g(x0, y0)
    vec2 br_rand_vec = hash(br); // g(x1, y0)
    vec2 tl_rand_vec = hash(tl); // g(x0, y1)
    vec2 tr_rand_vec = hash(tr); // g(x1, y1)

    vec4 norm = taylorInvSqrt(vec4(dot(bl_rand_vec, bl_rand_vec), dot(br_rand_vec, br_rand_vec), dot(tl_rand_vec, tl_rand_vec), dot(tr_rand_vec, tr_rand_vec)));
    bl_rand_vec*=norm.x;
    br_rand_vec*=norm.y;
    tl_rand_vec*=norm.z;
    tr_rand_vec*=norm.w;

    // Dot product to get scalar values for the corners
    float bl_scalar = dot(bl_rand_vec, bl_diff); // s
    float br_scalar = dot(br_rand_vec, br_diff); // t
    float tl_scalar = dot(tl_rand_vec, tl_diff); // u
    float tr_scalar = dot(tr_rand_vec, tr_diff); // v

    // Smooth interpolation vectors (merge bottoms together, merge tops, then merge the mergings)
    float bottom_mix = mix(bl_scalar, br_scalar, smooth_interpolation(uv.x));
    float top_mix = mix(tl_scalar, tr_scalar, smooth_interpolation(uv.x));
    float noise = mix(bottom_mix, top_mix, smooth_interpolation(uv.y));

    color = vec3(pow(noise, 0.15));
}

// @TODO changer ????
// Pseudo-random hash function by Pietro De Nicola
vec2 hash(vec2 p){
    vec2 d = vec2( dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3)));
    return fract(sin(d)*43758.5453);
    //return vec2(1, 1);
}

float rand(vec2 c){
    return fract(sin(dot(c.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

// Interpolation function (given in slide 12)
float smooth_interpolation(float t){
    return 6.*pow(t, 5) - 15.*pow(t, 4) + 10.*pow(t, 3);
}

// Mix function (slide 12)
float mix(float x, float y, float alpha){
    return (1-alpha)*x + alpha*y;
}


vec4 taylorInvSqrt(vec4 r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}
