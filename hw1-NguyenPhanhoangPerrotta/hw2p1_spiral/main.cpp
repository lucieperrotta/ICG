// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>

#include "triangle/triangle.h"

Triangle triangle;

void Init() {
    // sets background color
    glClearColor(0.937, 0.937, 0.937 /*gray*/, 1.0 /*solid*/);

    triangle.Init();
}

void spiral(glm::mat4 R, glm::mat4 S, glm::mat4 T){
    for(int i = 0; i < 60; ++i){

        R[0][0] = cos(0.3*i);
        R[0][1] = sin(0.3*i);
        R[1][0] = -sin(0.3*i);
        R[1][1] = cos(0.3*i);

        S[0][0] = 0.02+0.001*i;
        S[1][1] = 0.02+0.001*i;

        T[3][0] = 0.02+0.013*i;

        // compute transformations here
        triangle.Draw(R*T*S);
    }
}

void fermat(glm::mat4 R, glm::mat4 S, glm::mat4 T){
    // Loop for drawing lots of triangles
    for(int i = 0; i < 500; ++i){

        // Rotation matrix (fermat formula says: angle = n*2.4 rad)
        R[0][0] = cos(2.4*i);
        R[0][1] = sin(2.4*i);
        R[1][0] = -sin(2.4*i);
        R[1][1] = cos(2.4*i);

        // Sizing matrix (arbitrary choose a nice looking size)
        S[0][0] = 0.02;
        S[1][1] = 0.02;

        // Translation matrix (fermat formula says: distance to center = cst*sqrt(n)
        T[3][0] = 0.03*sqrt(i);
        T[3][1] = 0.03*sqrt(i);

        // compute transformations here
        triangle.Draw(R*T*S);
    }
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Init all as identity matrices
    glm::mat4 R = IDENTITY_MATRIX;
    glm::mat4 S = IDENTITY_MATRIX;
    glm::mat4 T = IDENTITY_MATRIX;

    // CHOOSE AMONG THE 2 SPIRAL TYPES
    spiral(R,S,T);
    //fermat(RST);
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main(int argc, char *argv[]) {
    // GLFW Initialization
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    glfwSetErrorCallback(ErrorCallback);

    // hint GLFW that we would like an OpenGL 3 context (at least)
    // http://www.glfw.org/faq.html#how-do-i-create-an-opengl-30-context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // attempt to open the window: fails if required version unavailable
    // note some Intel GPUs do not support OpenGL 3.2
    // note update the driver of your graphic card
    GLFWwindow* window = glfwCreateWindow(512, 512, "spiral", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for escape key
    glfwSetKeyCallback(window, KeyCallback);

    // GLEW Initialization (must have a context)
    // https://www.opengl.org/wiki/OpenGL_Loading_Library
    glewExperimental = GL_TRUE; // fixes glew error (see above link)
    if(glewInit() != GLEW_NO_ERROR) {
        fprintf( stderr, "Failed to initialize GLEW\n");
        return EXIT_FAILURE;
    }

    cout << "OpenGL" << glGetString(GL_VERSION) << endl;

    // initialize our OpenGL program
    Init();

    // render loop
    while(!glfwWindowShouldClose(window)) {
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    triangle.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
