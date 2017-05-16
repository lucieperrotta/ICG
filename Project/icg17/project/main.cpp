// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"
#include "glm/gtc/matrix_transform.hpp"

#include "framebuffer.h"
#include "trackball.h"

#include "grid/grid.h"
#include "noise/noise.h"
#include "water/water.h"
#include "sky/sky.h"

#include "displaytexture/displaytexture.h"

#undef M_PI
#define M_PI 3.14159
float currentAngle = 0;

int window_width = 800;
int window_height = 600;

FrameBuffer framebuffer;
FrameBuffer waterFramebuffer;
Noise noise;
Grid grid;
Water water;
Sky sky;

Trackball trackball;
float previousZ = 0.;

DisplayTexture displayTexture1;
DisplayTexture displayTexture2;

using namespace glm;

float window_ratio = window_width / (float) window_height;

float lake_level = 0.2f;
float height_scale = 0.7;
int LengthSegmentArea = 2; // grid side length

vec3 defaultCamPos = vec3(0.1, 0.5f, 0.0f);
vec3 defaultCamLook = vec3(0.0f, 0.5f, 0.0f);

vec3 cam_pos = defaultCamPos;
vec3 cam_look = defaultCamLook;
vec3 cam_up = vec3(0.0f, 1.0f, 0.0f);

mat4 projection_matrix;
mat4 view_matrix;
//mat4 quad_model_matrix = translate(mat4(1.0f), vec3(0.0f, -0.0f, 0.0f));
mat4 quad_model_matrix = IDENTITY_MATRIX;
mat4 trackball_matrix;
mat4 old_trackball_matrix;

vec2 offset = vec2(0., 0.);

// [navigation, fps, bezier]
float bezierLimit = 80; // limit
float bezierCount = 0;

// camera mode
vec3 cameraStatus = vec3(1,0,0);

void setMVPmatrices() {
    // setup view and projection matrices
    view_matrix = lookAt(cam_pos, cam_look, cam_up);
    projection_matrix = perspective(45.0f, window_ratio, 0.1f, 10.0f);
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

    water.Init(water_texture_id, LengthSegmentArea, lake_level);
    noise.Init(window_width, window_height, framebuffer_texture_id);
    grid.Init(framebuffer_texture_id, lake_level, height_scale, LengthSegmentArea);
    sky.Init();

    displayTexture1.Init(0, 0);
    displayTexture2.Init(0, 0.5f);

    // trackball
    trackball_matrix = IDENTITY_MATRIX;

}

vec3 bezierCurves(float time){

    // deCasteljau algorithm
    vec3 b0 = vec3(1.5f, 1.5f, 0.0f);
    vec3 b1 = vec3(4,1.5f,0);
    vec3 b2 = vec3(4,1.5f,3);

    // make it work until s seconds -> time should go from 0 to 1
    float t = time/bezierLimit;

    vec3 b0_1 = (1-t)*b0 + t*b1;
    vec3 b1_1 = (1-t)*b1 + t*b2;
    vec3 b0_2 = (1-t)*b0_1 + t*b1_1;

    return b0_2;
}

void Display() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float time = glfwGetTime();

    // BEZIER CURVES
    bezierCount += 0.1;
    if(cameraStatus.z == 1){
        cam_pos = bezierCurves(bezierCount);
    }
    // begin again if go too far
    bezierCount = (bezierCount > bezierLimit) ? 0 : bezierCount;


    // set up matrices for MVP
    setMVPmatrices();

    // heightmap for grid
    framebuffer.Bind();
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        noise.Draw(offset);
    }
    framebuffer.Unbind();

    // water reflection
    waterFramebuffer.Bind();
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // inverse cam position and look up depending on lake level
        vec3 cam_pos_mirror = cam_pos;
        cam_pos_mirror.y = cam_pos.y - 2*abs(cam_pos.y-lake_level);
        vec3 cam_look_mirror = cam_look;
        cam_look_mirror.y = cam_look.y + 2*abs(cam_look.y-lake_level);
        mat4 view_matrix_mirror = lookAt(cam_pos_mirror, cam_look_mirror, cam_up);

        mat4 axis_invert = mat4(vec4(-1,0,0,0),
                                vec4(0,1,0,0),
                                vec4(0,0,1,0),
                                vec4(0,0,0,1));

        sky.Draw(time, quad_model_matrix, inverse(trackball_matrix*axis_invert)*view_matrix_mirror, projection_matrix);
        grid.Draw(cam_pos, time, quad_model_matrix, trackball_matrix*view_matrix_mirror, projection_matrix);
    }
    waterFramebuffer.Unbind();

    // render to Window
    glViewport(0, 0, window_width, window_height);

    sky.Draw(time, quad_model_matrix, trackball_matrix*view_matrix, projection_matrix);
    grid.Draw(cam_pos, time, quad_model_matrix, trackball_matrix*view_matrix, projection_matrix);
    grid.Draw(cam_pos, time, quad_model_matrix, trackball_matrix*view_matrix, projection_matrix,0);
    water.Draw(time, quad_model_matrix, trackball_matrix*view_matrix, projection_matrix);

    //displayTexture1.Draw();
    //displayTexture2.Draw();
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

    float window_ratio = window_width / (float) window_height;
    projection_matrix = perspective(45.0f, window_ratio, 0.1f, 10.0f);

    glViewport(0, 0, window_width, window_height);

    // when the window is resized, the framebuffer and the screenquad should also be resized
    framebuffer.Cleanup();
    framebuffer.Init(window_width, window_height);
    noise.UpdateSize(window_width, window_height);
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


    float delta = 0.65;
    float deltaXY = 0.03;
    float delta_offset = 0.1;
    float deltaLR = 0.05;
    float deltaLook = 0.01;
    vec3 direction = cam_look - cam_pos;
    vec2 dir = vec2(direction.x,direction.z);
    vec3 cross_dir = cross(direction, vec3(0.0,1.0,0));
    vec2 cross_dir2 = vec2(cross_dir.x, cross_dir.z);

    // TRIED TO GET HEIGHTMAP
    const size_t n_values = window_width*window_height*3;
    GLubyte* pixels = new GLubyte [n_values];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);

    // @TODO add inertie : ie fadein/out movement -> bezier ! et c'est un bonus en plus d'utiliser bezier
    switch(key) {
    case 49: //1 - navigation mode
        cameraStatus = vec3(1,0,0);
        cam_pos = defaultCamPos;
        cam_look = defaultCamLook;
        break;
    case 50: //2 - fps mode
        cameraStatus = vec3(0,1,0);
        cam_pos = defaultCamPos;
        cam_look = defaultCamLook;
        break;
    case 51: //3 - bezier curves
        cameraStatus = vec3(0,0,1);
        bezierCount = 0;
        break;
    case GLFW_KEY_LEFT:
        if(cameraStatus.x == 1){
            offset+= vec2(-delta_offset, delta_offset)*cross_dir2;
        }
        break;
    case GLFW_KEY_RIGHT:
        if(cameraStatus.x == 1){
            offset-= vec2(-delta_offset, delta_offset)*cross_dir2;
        }
        break;
    case GLFW_KEY_DOWN:
        if(cameraStatus.x == 1){
            offset+= vec2(-delta_offset, delta_offset)*dir;
            cam_pos.y -= delta*direction.y;
            cam_look.y -= delta*direction.y;
        }
        else if(cameraStatus.y == 1){
            int pos = 3*(window_width/2*window_height + window_width/2);
            std::cout << pixels[pos];
            //cam_pos.y = pixels[pos];
        }
        break;
    case GLFW_KEY_UP:
        if(cameraStatus.x == 1){
            offset -= vec2(-delta_offset, delta_offset)*dir;
            cam_pos.y += delta*direction.y;
            cam_look.y += delta*direction.y;
        }
        break;
    case 65: // A
        cam_look -= cross(direction, vec3(0.0,1.0,0))*deltaLR;
        break;
    case 68: // D
        cam_look += cross(direction, vec3(0.0,1.0,0))*deltaLR;
        break;
    case 87: // W
        cam_look += vec3(0,deltaLook,0);
        break;
    case 83: // S
        cam_look -= vec3(0,deltaLook,0);
        break;
    case 88: // X
        if(cameraStatus.x == 1){
            cam_look -= vec3(0,deltaXY,0);
            cam_pos -= vec3(0,deltaXY,0);
        }
        break;

    case 90: // Y
        if(cameraStatus.x == 1){
            cam_look += vec3(0,deltaXY,0);
            cam_pos += vec3(0,deltaXY,0);
        }
        break;
    case 81: // Q
        break;
    case 69: // E
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
    noise.Cleanup();
    water.Cleanup();
    sky.Cleanup();
    displayTexture1.Cleanup();
    displayTexture2.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
