#version 330

uniform mat4 M;
out vec3 fcolor;
in vec3 vpoint;

// Unused to many times computed old matrix (new one is in main.cpp init)
/*
mat4 T = mat4(
1,0,0,0,
0,1,0,0,
0,0,1,0,
0,0,0,1
);
*/

void main() {
    gl_Position = M*vec4(vpoint, 1.0);
    if(gl_VertexID==0){
    fcolor = vec3(1.0,0.0,0.0);
    }

    if(gl_VertexID==1){
    fcolor = vec3(0.0,1.0,0.0);
    }

    if(gl_VertexID==2){
    fcolor = vec3(0.0,0.0,1.0);
    }
}
