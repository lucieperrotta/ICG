#version 330 core
in vec2 uv;
uniform sampler2D colorTex;
uniform sampler2D velocityTex;
out vec4 color;

void main() {
    /// TODO: use the velocity vector stored in velocityTex to compute the line integral
    /// TODO: use a constant number of samples for integral (what happens if you take too few?)
    /// HINT: you can scale the velocity vector to make the motion blur effect more prominent
    /// HINT: to debug integration don't use the velocityTex, simply assume velocity is constant, e.g. vec2(1,0)
    vec4 colorCurrent =texture(colorTex, uv);
    int N = 4; // size of average
    vec2 velocity = texture(velocityTex, uv).xy;

    // Actual C_new computation
    for(int i = 1; i < N; i++) {
        vec4 ith_pixel_color = texture(colorTex, uv + (i * velocity)); // compute the ith pixel color
        colorCurrent += ith_pixel_color; // add it to our color sum
    }

    color = colorCurrent/N; // do the average
}
