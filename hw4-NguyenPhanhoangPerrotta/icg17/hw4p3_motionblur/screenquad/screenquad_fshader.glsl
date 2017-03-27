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

    vec2 velocity = vec2(0,2);

    int N = 2;
    vec4 colorCurrent = texture(colorTex, uv);
    vec4 colorNext;
    for(int i = 0; i < N; i++) {
        colorNext = texture(colorTex, vec2(uv.x + velocity.x*i, uv.y + velocity.y*i));
        colorCurrent += colorNext;
        /*colorCurrent.x += colorNext.x;
        colorCurrent.y += colorNext.y;
        colorCurrent.z += colorNext.z;
        colorCurrent.w += colorNext.w;*/
    }
    colorCurrent /= N;
    /*colorCurrent.x = colorCurrent.x/N;
    colorCurrent.y = colorCurrent.y/N;
    colorCurrent.z = colorCurrent.z/N;
    colorCurrent.w = colorCurrent.w/N;*/

    color = colorCurrent;

    //color = texture(colorTex, uv);

}
