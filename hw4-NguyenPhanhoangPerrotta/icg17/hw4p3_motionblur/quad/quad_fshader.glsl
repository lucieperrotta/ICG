#version 330 core
in vec2 uv;
in vec4 currentPosition;
in vec4 previousPosition;

uniform mat4 currentMVP;
uniform mat4 previousMVP;

uniform sampler2D tex;

layout (location = 0) out vec4 color;
layout (location = 1) out vec2 motionVector;

void main() {
    color = texture(tex, uv);
//#define STRIP_CODE
    // TODO: compute the screen space motion vector (in pixels!)
    // HINT: use straightforward finite differences and assume unit time-step
    // HINT: how do you compute pixels positions given homogeneous coordinate? (x,y,z,w)

    vec4 inverseCurrent = inverse(currentMVP) * currentPosition;
    vec4 inversePrev = inverse(previousMVP) * previousPosition;

    vec2 res = vec2(
                inverseCurrent.x - inversePrev.x,
                inverseCurrent.y - inversePrev.y
                );
    motionVector = res;
}
