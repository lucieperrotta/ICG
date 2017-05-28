#version 330

in vec3 vpoint;
in vec2 vtexcoord;

out vec2 uv;

uniform mat4 MVP;
uniform float time;

mat4 T(float tx, float ty, float tz) {
    mat4 T = mat4(1);
    T[3][0] = tx;
    T[3][1] = ty;
    T[3][2] = tz;
    return T;
}

mat4 S(float tx, float ty, float tz){
    mat4 T = mat4(1);
    T[0][0] = tx;
    T[1][1] = ty;
    T[2][2] = tz;
    return T;
}

mat4 Ry(float alpha){
    mat4 T = mat4(1);
    T[0][0] = cos(alpha);
    T[2][0] = sin(alpha);
    T[0][2] = -sin(alpha);
    T[2][2] = cos(alpha);
    return T;
}

void main() {
    gl_Position =  MVP * T(0,1,0) * S(6,6,6) * Ry(time/100) *vec4(vpoint,1);

    uv = vtexcoord;
}
