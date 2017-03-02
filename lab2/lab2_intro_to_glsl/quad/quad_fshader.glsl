#version 330
out vec3 color;

const vec3 COLORS[2] = vec3[](
        vec3(1.0,0.0,0.0),
vec3(0.0,1.0,0.0)
);


void main() {
    color = COLORS[gl_PrimitiveID];
}
