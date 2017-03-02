#version 330


in vec3 fcolor;
out vec3 color; // output color

void main() {
    color = fcolor; // color is red for every fragment
}
