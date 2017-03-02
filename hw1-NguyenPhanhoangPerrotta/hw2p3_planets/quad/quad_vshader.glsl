#version 330

in vec3 vpoint;
in vec2 vtexcoord;

out vec2 uv;

uniform mat4 MVP;

void main() {

    // scale
    mat3 S = mat3(.25);

    // rotation
    mat3 R = mat3(1);
    float alpha = radians(30 /*degrees*/);
    R[0][0] =  cos(alpha);
    R[0][1] =  sin(alpha);
    R[1][0] = -sin(alpha);
    R[1][1] =  cos(alpha);

    // translation
    mat4 T = mat4(1);
    T[3][0] = .5;
    T[3][1] = .5;

    gl_Position = MVP * mat4(S) * vec4(vpoint, 1.0);
    uv = vtexcoord;
}
