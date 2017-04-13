#version 330

in vec2 uv;
out vec3 color;

uniform sampler2D tex;
uniform float tex_width;
uniform float tex_height;

// Functions headers
float ridged_fBm(vec2 point, float H, float lacunarity, int octaves, float gain);
float fBm(vec2 point, float H, float lacunarity, int octaves, float gain);
float perlin(vec2 uv);
vec2 hash(vec2 p);
float smooth_interpolation(float t);
float mix(float x, float y, float alpha);

void main() {

    color=vec3(fBm(uv, 0.9, 1.8, 20, 0.6)+0.05);
    //color=vec3(ridged_fBm(uv, 1.2, 1.8, 100, 1));
}

float fBm(vec2 point, float H, float lacunarity, int octaves, float gain){
    float value = 0.0;
    /* inner loop of fractal construction */
    for (int i = 0; i < octaves; i++) {
        value += gain * perlin(point) * pow(lacunarity, -H*i);
        //value += perlin(point) * 0.7;
        point *= lacunarity;
    }
    return value;
}

float ridged_fBm(vec2 point, float H, float lacunarity, int octaves, float gain){
    float value = 0.0;
    /* inner loop of fractal construction */
    for (int i = 0; i < octaves; i++) {
        value += gain * (1.0 - abs(perlin(point))) * pow(lacunarity, -H*i);
        //value += perlin(point) * 0.7;
        point *= lacunarity;
    }
    return value;
}

float perlin(vec2 uv){
    // Grid size in squares
    float N = 2.;

    // Coordinates definitions
    vec2 checker_coord = uv*N; // Global big checkerboord coordinates
    vec2 colums_line_number = floor(checker_coord); // "Line/column #" coordinate
    vec2 coord_frac = fract(checker_coord); // Relative-to-local-rectangle coordinate

    // Compute the pixel values [0, 1] of the 4 corners of the square
    vec2 bl = (colums_line_number+vec2(0.,0.) );
    vec2 br = (colums_line_number+vec2(1.,0.) );
    vec2 tl = (colums_line_number+vec2(0.,1.) );
    vec2 tr = (colums_line_number+vec2(1.,1.) );

    // Compute difference vectors from corners to actual point
    vec2 bl_diff = checker_coord - bl; // a
    vec2 br_diff = checker_coord - br; // b
    vec2 tl_diff = checker_coord - tl; // c
    vec2 tr_diff = checker_coord - tr; // d

    // Generate random vector for each corner
    vec2 bl_rand_vec = hash(bl); // g(x0, y0)
    vec2 br_rand_vec = hash(br); // g(x1, y0)
    vec2 tl_rand_vec = hash(tl); // g(x0, y1)
    vec2 tr_rand_vec = hash(tr); // g(x1, y1)

    // Dot product to get scalar values for the corners
    float bl_scalar = dot(bl_rand_vec, bl_diff); // s
    float br_scalar = dot(br_rand_vec, br_diff); // t
    float tl_scalar = dot(tl_rand_vec, tl_diff); // u
    float tr_scalar = dot(tr_rand_vec, tr_diff); // v

    // Smooth interpolation vectors (merge bottoms together, merge tops, then merge the mergings)
    float bottom_mix = mix(bl_scalar, br_scalar, smooth_interpolation(coord_frac.x));
    float top_mix = mix(tl_scalar, tr_scalar, smooth_interpolation(coord_frac.x));
    float noise = mix(bottom_mix, top_mix, smooth_interpolation(coord_frac.y));

    // Noise is between -1 and 1 and we want it between 0 and 1
    return (noise+1)/2;
}

// Pseudo-random hash function by Pietro De Nicola (vectors coord between -1 and 1)
vec2 hash(vec2 p){
    vec2 d = vec2( dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3)));
    return (fract(sin(d)*43758.5453)-0.5)*2.;
}

// Interpolation function (given in slide 12)
float smooth_interpolation(float t){
    return 6.*pow(t, 5) - 15.*pow(t, 4) + 10.*pow(t, 3);
}

// Mix function (slide 12)
float mix(float x, float y, float alpha){
    return (1-alpha)*x + alpha*y;
}

/* ULTIMATE DEBUG TECHNIQUE
if (bl_diff.x < -1.0 || bl_diff.y < -1.0 || bl_diff.x > 1.0 || bl_diff.y > 1.0 ) {
    color = vec3(1, 0, 0);
    return;
}
*/
