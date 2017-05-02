// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"
#include "glm/gtc/matrix_transform.hpp"

#include "framebuffer.h"
#include "trackball.h"

#include "grid/grid.h"
#include "screenquad/screenquad.h"
#include "water/water.h"
#include "sky/sky.h"

#include "displaytexture/displaytexture.h"


int window_width = 800;
int window_height = 600;

FrameBuffer framebuffer;
FrameBuffer waterFramebuffer;
ScreenQuad screenquad;
Grid grid;
Water water;
Sky sky;

Trackball trackball;
float previousZ = 0.;

DisplayTexture displayTexture1;
DisplayTexture displayTexture2;

using namespace glm;

float ratio = window_width / (float) window_height;

vec3 cam_pos = vec3(2.0f, 2.0f, 2.0f);
vec3 cam_look = vec3(0.0f, 0.0f, 0.0f);
vec3 cam_up = vec3(0.0f, 1.0f, 0.0f);

mat4 projection_matrix;
mat4 view_matrix;
mat4 quad_model_matrix = translate(mat4(1.0f), vec3(0.0f, -0.25f, 0.0f));
mat4 trackball_matrix;
mat4 old_trackball_matrix;


void setMVPmatrices() {
    // setup view and projection matrices
    view_matrix = lookAt(cam_pos, cam_look, cam_up);
    projection_matrix = perspective(45.0f, ratio, 0.1f, 10.0f);
}


mat4 PerspectiveProjection(float fovy, float aspect, float near, float far) {
    float top = near*tan(fovy/2.f);
    float bottom = -top;
    float right = top*aspect;
    float left = -right;

    mat4 perspective = mat4(1.0f);
    perspective[0][0] = 2.f*near/(right-left);
    perspective[1][1] = 2.f*near/(top-bottom);
    perspective[2][0] = (right+left)/(right-left);
    perspective[2][1] = (top+bottom)/(top-bottom);
    perspective[2][2] = -(far+near)/(far-near);
    perspective[2][3] = -1.f;
    perspective[3][2] = -(2.f*far+near)/(far-near);
    perspective[3][3] = 0;

    return perspective;
}


void Init(GLFWwindow* window) {
    // sets background color
    glClearColor(1,1, 1, 1.0);
    glEnable(GL_DEPTH_TEST);

    // on retina/hidpi displays, pixels != screen coordinates
    glfwGetFramebufferSize(window, &window_width, &window_height);

    GLuint framebuffer_texture_id = framebuffer.Init(window_width, window_height);
    GLuint water_texture_id = waterFramebuffer.Init(window_width, window_height);

    water.Init(water_texture_id);
    screenquad.Init(window_width, window_height, framebuffer_texture_id);
    grid.Init(framebuffer_texture_id);
    sky.Init();

    displayTexture1.Init(20, 0);
    displayTexture2.Init(10, 0.5f);

    // trackball
    trackball_matrix = IDENTITY_MATRIX;

}


void Display() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    double time = 0;  //glfwGetTime();

    setMVPmatrices();

    // heightmap for grid
    framebuffer.Bind();
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        screenquad.Draw();
    }
    framebuffer.Unbind();

    // water reflection
    waterFramebuffer.Bind();
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        vec3 cam_pos_mirror = cam_pos;
        cam_pos_mirror.y = -cam_pos_mirror.y;
        mat4 view_matrix_mirror = lookAt(cam_pos_mirror, cam_look, cam_up);
        grid.Draw(time, quad_model_matrix, view_matrix_mirror, projection_matrix);
    }
    waterFramebuffer.Unbind();

    // render to Window
    glViewport(0, 0, window_width, window_height);

    sky.Draw(time, quad_model_matrix, trackball_matrix*view_matrix, projection_matrix);
    grid.Draw(time, quad_model_matrix, trackball_matrix*view_matrix, projection_matrix, 1);
    water.Draw(time, quad_model_matrix, trackball_matrix*view_matrix, projection_matrix);

    displayTexture1.Draw();
    displayTexture2.Draw();

}


// transforms glfw screen coordinates into normalized OpenGL coordinates.
vec2 TransformScreenCoords(GLFWwindow* window, int x, int y) {
    // the framebuffer and the window doesn't necessarily have the same size
    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);
    return vec2(2.0f * (float)x / width - 1.0f, 1.0f - 2.0f * (float)y / height);
}

void MouseButton(GLFWwindow* window, int button, int action, int mod) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double x_i, y_i;
        glfwGetCursorPos(window, &x_i, &y_i);
        vec2 p = TransformScreenCoords(window, x_i, y_i);
        trackball.BeingDrag(p.x, p.y);
        old_trackball_matrix = trackball_matrix;
    }
}


void MousePos(GLFWwindow* window, double x, double y) {
    vec2 p = TransformScreenCoords(window, x, y);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double x_i, y_i;
        glfwGetCursorPos(window, &x_i, &y_i);
        p = TransformScreenCoords(window, x_i, y_i);
        trackball_matrix = trackball.Drag(p.x,p.y) * old_trackball_matrix;
    }

    // zoom
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        vec3 newVec = vec3(0., 0., p.y-previousZ);
        view_matrix=translate(view_matrix, newVec);
    }
    previousZ = p.y; // for not going infinitely fast
}


// Gets called when the windows/framebuffer is resized.
void SetupProjection(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;

    cout << "Window has been resized to " << window_width << "x" << window_height << "." << endl;

    glViewport(0, 0, window_width, window_height);

    projection_matrix = PerspectiveProjection(45.0f, (GLfloat)window_width / window_height, 0.1f, 100.f);
}


// gets called when the windows/framebuffer is resized.
void ResizeCallback(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;

    float ratio = window_width / (float) window_height;
    projection_matrix = perspective(45.0f, ratio, 0.1f, 10.0f);

    glViewport(0, 0, window_width, window_height);

    // when the window is resized, the framebuffer and the screenquad should also be resized
    framebuffer.Cleanup();
    framebuffer.Init(window_width, window_height);
    screenquad.UpdateSize(window_width, window_height);
}


void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}


void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // escape to close window
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    // only act on release
    if(action != GLFW_RELEASE) {
        return;
    }

    float delta = 0.03;

    switch(key) {
    case GLFW_KEY_LEFT:
        cam_pos -= vec3(delta,0,-delta);
        cam_look -= vec3(delta,0,-delta);
        break;
    case GLFW_KEY_RIGHT:
        cam_pos += vec3(delta,0,-delta);
        cam_look += vec3(delta,0,-delta);
        break;
    case GLFW_KEY_DOWN:
        cam_pos += vec3(delta,0.0,delta);
        cam_look += vec3(delta,0.0,delta);
        break;
    case GLFW_KEY_UP:
        cam_pos -= vec3(delta,0.0,delta);
        cam_look -= vec3(delta,0.0,delta);
        break;
    default:
        break;
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // attempt to open the window: fails if required version unavailable
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "framebuffer", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // keyboard controls
    glfwSetKeyCallback(window, KeyCallback);

    // set the framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, ResizeCallback);

    // set the framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, SetupProjection);

    // set the mouse press and position callback
    glfwSetMouseButtonCallback(window, MouseButton);
    glfwSetCursorPosCallback(window, MousePos);

    // GLEW Initialization (must have a context)
    glewExperimental = GL_TRUE; // fixes glew error (see above link)
    if(glewInit() != GLEW_NO_ERROR) {
        fprintf( stderr, "Failed to initialize GLEW\n");
        return EXIT_FAILURE;
    }

    cout << "OpenGL" << glGetString(GL_VERSION) << endl;

    // initialize our OpenGL program
    Init(window);


    // render loop
    while(!glfwWindowShouldClose(window)){
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // cleanup
    grid.Cleanup();
    framebuffer.Cleanup();
    screenquad.Cleanup();
    water.Cleanup();
    sky.Cleanup();
    displayTexture1.Cleanup();
    displayTexture2.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
