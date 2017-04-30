#version 330

in vec2 uv;
out vec3 color;

uniform sampler2D tex;
uniform float tex_width;
uniform float tex_height;

// Functions headers
float ridged_fBm(vec2 point, float H, float lacunarity, int octaves, float gain, float offset);
float fBm(vec2 point, float H, float lacunarity, int octaves, float gain, float offset);
float HybridMultifractal(vec2 point, float H, float lacunarity, float octaves, float offset);

float perlin(vec2 uv);
float simplex(vec2 uv);

vec2 hash(vec2 p);
float smooth_interpolation(float t);
float mix(float x, float y, float alpha);

void main() {
    color=vec3(ridged_fBm(uv, 1.5, 1.5, 10, 0.5, 0.0));
    //color = vec3(HybridMultifractal(uv, 0.1, 3.7, 2.0, 0.));
}

float fBm(vec2 point, float H, float lacunarity, int octaves, float gain, float offset){
    float value = 0.0;
    /* inner loop of fractal construction */
    for (int i = 0; i < octaves; i++) {
        value += gain * perlin(point) * pow(lacunarity, -H*i);
        value -= offset;
        point *= lacunarity;
    }
    return value;
}

float ridged_fBm(vec2 point, float H, float lacunarity, int octaves, float gain, float offset){
    float value = 0.0;
    /* inner loop of fractal construction */
    for (int i = 0; i < octaves; i++) {
        value += gain * (1.0 - abs(simplex(point)))* pow(lacunarity, -H*i);
        value-=offset;
        point *= lacunarity;
    }
    return value;
}

float HybridMultifractal(vec2 point, float H, float lacunarity, float octaves, float offset){
    float frequency, result, signal, weight, remainder;
    int i;
    bool first = true;
    float exponent_array[10]; //octaves_max = 10;

    /* precompute and store spectral weights */
    if (first) {
        /* seize required memory for exponent_array */
        frequency = 1.0;
        for (i = 0; i<octaves; i++) {
            /* compute weight for each frequency */
            exponent_array[i] = pow(frequency, -H);
            frequency *= lacunarity;
        }
        first = false;
    }

    /* get first octave of function */
    result = (perlin(point) + offset) * exponent_array[0];
    weight = result;

    /* increase frequency */
    point[0] = point[0] * lacunarity;
    point[1] = point[1] * lacunarity;

    /* spectral construction inner loop, where the fractal is built */
    for (i = 1; i<octaves; i++) {
        /* prevent divergence */
        if (weight > 1.0) weight = 1.0;
        /* get next higher frequency */
        signal = (perlin(point) + offset) * exponent_array[i];
        /* add it in, weighted by previous freq's local value */
        result += weight * signal;
        /* update the (monotonically decreasing) weighting value */
        /* (this is why H must specify a high fractal dimension) */
        weight *= signal;
        /* increase frequency */
        point[0] = point[0] * lacunarity;
        point[1] = point[1] * lacunarity;
    }

    /* take care of remainder in “octaves” */
    remainder = octaves - int(octaves);
    if (remainder != 0)
        /* “i” and spatial freq. are preset in loop above */
        result += remainder * perlin(point) * exponent_array[i];
    return(result);
}

// Inspired by Ashima
float simplex(vec2 uv){

    // Corner 1
    vec2 x0 = floor(uv + (uv.x+uv.y)*(0.5*(sqrt(3.0)-1.0)));
    vec2 p0 = uv - x0 + (x0.x+x0.y)*0.211324865f; // 0.211324865f = (3 - sqrt(3)) / 6

    // Corner 2
    vec2 x1;
    if (p0.x > p0.y){ // determine if x1 is in lower or upper simplex
        x1= vec2(1.0, 0.0); // lower
    }else{
        x1 =vec2(0.0, 1.0); // upper
    };
    vec2 p1 = p0 + 0.211324865f - x1; // 0.211324865f = (3 - sqrt(3)) / 6

    // Corner 3
    vec2 p2 = p0 -1.0 + 2.0 * 0.211324865f; // 0.211324865f = (3 - sqrt(3)) / 6

    // Offset
    x0 = x0 - floor(x0 * (1.0 / 289.0)) * 289.0; // mod 289
    vec3 offset = x0.y + vec3(0.0, x1.y, 1.0 );

    // First iteration
    offset = ((offset*34.0)+1.0)*offset;
    offset = offset - floor(offset * (1.0 / 289.0)) * 289.0;

    offset = offset + x0.x + vec3(0.0, x1.x, 1.0 );

    // Second iteration
    offset = ((offset*34.0)+1.0)*offset;
    offset = offset - floor(offset * (1.0 / 289.0)) * 289.0;

    // p0 is computed independently
    float fr0 = 2.0 * fract(offset.x * 1./41.) - 1.0;
    float p0y = abs(fr0) - 0.5;
    float p0x = fr0 - floor(fr0 + 0.5);

    float f0 = p0x  * p0.x  +p0y  * p0.y;

    // p1 and p2 are computed together
    float fr1 = 2.0 * fract(offset.y * 1./41.) - 1.0;
    float p1y = abs(fr1) - 0.5;
    float p1x = fr1 - floor(fr1 + 0.5);

    float fr2 = 2.0 * fract(offset.z * 1./41.) - 1.0;
    float p2y = abs(fr2) - 0.5;
    float p2x = fr2 - floor(fr2 + 0.5);

    float f1 = (vec2(p1x, p2x) * vec2(p1.x, p2.x) + vec2(p1y, p2y) * vec2(p1.y, p2.y)).x;
    float f2 = (vec2(p1x, p2x) * vec2(p1.x, p2.x) + vec2(p1y, p2y) * vec2(p1.y, p2.y)).y;

    // Contributions of all 3 corners
    vec3 final = vec3(dot(p0,p0), dot(p1,p1), dot(p2,p2));
    final = 0.5 - final;

    int FLATNESS = 2;
    for(int i=0; i<FLATNESS; ++i){
        final *= final;
    }
    final *= 1.7928429140015f - 0.8537347209531f * ( vec3(p0x, p1x, p2x)*vec3(p0x, p1x, p2x) + vec3(p0y, p1y, p2y)*vec3(p0y, p1y, p2y) ); // Numerical approximation of inverse sqrt by Stefan Gustavson

    // Noise is between 0 and 1/130 and we want it between 0 and 1
    return 130.0 * dot(final, vec3(f0, f1, f2));
}

float perlin(vec2 uv){
    // Grid size in squares
    float N = 4.;

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
