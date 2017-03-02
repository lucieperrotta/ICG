#version 330

in vec3 vpoint;
out vec3 fcolor;


void main() {
    gl_Position = vec4(vpoint, 1.0);
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
