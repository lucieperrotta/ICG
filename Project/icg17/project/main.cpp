// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"
#include "glm/gtc/matrix_transform.hpp"

#include "framebuffer.h"
#include "grid/grid.h"
#include "noise/noise.h"
#include "water/water.h"
#include "sky/sky.h"

#undef M_PI
#define M_PI 3.14159

using namespace glm;

int window_width = 1200;
int window_height = 800;
float window_ratio = window_width / (float) window_height;

// OBJECTS
FrameBuffer framebuffer;
FrameBuffer waterFramebuffer;
FrameBuffer snowFramebuffer;
Noise noise;
Noise snowNoise;
Grid grid;
Water water;
Sky sky;

// MATRIX PROJECTION
mat4 projection_matrix;
mat4 view_matrix;
//mat4 quad_model_matrix = translate(mat4(1.0f), vec3(0.0f, -0.0f, 0.0f));
mat4 quad_model_matrix = IDENTITY_MATRIX;

// LIMITS
float lake_level = 0.3f;
float sky_limit = 1.f;
float height_scale = 0.7f;
int LengthSegmentArea = 4; // grid side length

// CAMERA PARAMETERS
vec3 defaultCamPos = vec3(0.0f, lake_level + 0.12f, 0.0f); // 0.1 0.5 0.0.0
vec3 defaultCamLook = vec3(0.1f, lake_level + 0.12f, 0.0f); // 0.0 0.5 0.0
vec3 cam_pos = defaultCamPos;
vec3 cam_look = defaultCamLook;
vec3 cam_up = vec3(0.0f, 1.0f, 0.0f);
vec3 cameraStatus = vec3(1,0,0);
float delta_offset = 0.1f; // unit of movement horizontally
float delta = 0.63f; // percentage of movement vertically
float delta_fps = 0.7f; // unit of movement fps

const int nb_pixels = 16; // nb pixel to get from height map to smooth fps camera movement

vec2 offset = vec2(0., 0.); // to create movement

// BEZIER PARAMETERS
int bezierFPSStatus = 0;
float bezierLimitPanorama = 200; // limit
float bezierCountPanorama = 0;
int stopPanorama = 0;
vec3 b0_fps;
vec3 b1_fps;
vec3 b2_fps;
vec2 tmp_offset;
float bezierLimitFPS = 1.5f; // duration time
float bezierCountFPS = 0.f;
float speedBezierFPS = 0.05f;

// setup view and projection matrices
void setMVPmatrices() {
    view_matrix = lookAt(cam_pos, cam_look, cam_up);
    projection_matrix = perspective(45.0f, window_ratio, 0.001f, 10.0f);
}

// create perpective matrix
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

// init elements
void Init(GLFWwindow* window) {
    // sets background color
    glClearColor(1, 1, 1, 1.0);
    glEnable(GL_DEPTH_TEST);

    // on retina/hidpi displays, pixels != screen coordinates
    glfwGetFramebufferSize(window, &window_width, &window_height);

    GLuint framebuffer_texture_id = framebuffer.Init(window_width, window_height);
    GLuint water_texture_id = waterFramebuffer.Init(window_width, window_height);
    GLuint snow_framebuffer_texture_id = snowFramebuffer.Init(window_width, window_height);

    water.Init(water_texture_id, LengthSegmentArea, lake_level);
    noise.Init(window_width, window_height, framebuffer_texture_id);
    grid.Init(framebuffer_texture_id, lake_level, height_scale, LengthSegmentArea);
    sky.Init();
}

// Bezier curves definition
vec3 bezierCurves(float time, float limit, vec3 b0, vec3 b1, vec3 b2){

    // make it work until s seconds -> time should go from 0 to 1
    float t = time/limit;

    // deCasteljau algorithm
    vec3 b0_1 = (1-t)*b0 + t*b1;
    vec3 b1_1 = (1-t)*b1 + t*b2;
    vec3 b0_2 = (1-t)*b0_1 + t*b1_1;

    return b0_2;
}

void Display() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float time = glfwGetTime();

    // PANORAMA CAMERA
    if(cameraStatus.z == 1){
        if(stopPanorama != 1) bezierCountPanorama += 0.1f;

        vec3 b0 = vec3(0,5,0);
        vec3 b1 = b0 + vec3(2,0,0);
        vec3 b2 = b1 + vec3(0,0,2);
        vec3 res = bezierCurves(bezierCountPanorama, bezierLimitPanorama, b0, b1, b2);
        offset = vec2(res.x, res.z);

        vec3 b0_look = defaultCamLook;
        vec3 b1_look = b0_look - vec3(0.1,0,0.1);
        vec3 b2_look = b1_look + vec3(0.1,0,0.1);
        vec3 res_look = bezierCurves(bezierCountPanorama, bezierLimitPanorama, b0_look, b1_look, b2_look);
        cam_look = res_look;

        // begin again if go too far
        bezierCountPanorama = (bezierCountPanorama > bezierLimitPanorama) ? 0 : bezierCountPanorama;
    }

    // FPS CAMERA
    if(bezierFPSStatus == 1){
        if(bezierCountFPS > bezierLimitFPS){
            bezierFPSStatus = 0;
            bezierCountFPS = 0;
        }
        else {
            bezierCountFPS += speedBezierFPS;
            vec3 res = bezierCurves(bezierCountFPS, bezierLimitFPS, b0_fps, b1_fps, b2_fps);
            offset = vec2(res.x, res.z);

            framebuffer.Bind();
            {
                int nb_pixels_by_side = sqrt(nb_pixels);
                GLfloat height[nb_pixels];
                glReadPixels(window_width/2 - nb_pixels_by_side/2, window_height/2 - nb_pixels_by_side/2, nb_pixels_by_side, nb_pixels_by_side, GL_RED, GL_FLOAT, &height);

                // get average value of heigthmap pixels
                float sum = 0;
                for(int i = 0;i < nb_pixels; i++){
                    sum += height[i];
                }
                float average = sum/(float)nb_pixels;

                // avoid to walk under lake
                if(average < lake_level) average = lake_level;

                float offset_height = 0.05f;

                float height_final = average + offset_height; // to be a bit higher
                cam_pos.y = height_final;
                cam_look.y = height_final;
            }
            framebuffer.Unbind();
        }
    }

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
        cam_look_mirror.y = cam_look.y + 2*(lake_level-cam_look.y);
        mat4 view_matrix_mirror = lookAt(cam_pos_mirror, cam_look_mirror, cam_up);

        sky.Draw(time, quad_model_matrix, view_matrix_mirror, projection_matrix);
        grid.Draw(cam_pos, time, offset, quad_model_matrix, view_matrix_mirror, projection_matrix);
    }
    waterFramebuffer.Unbind();

    snowFramebuffer.Bind();
    {
        snowNoise.Draw();
    }
    snowFramebuffer.Unbind();

    // render to Window
    glViewport(0, 0, window_width, window_height);

    sky.Draw(time, quad_model_matrix, view_matrix, projection_matrix);
    grid.Draw(cam_pos, time, offset, quad_model_matrix, view_matrix, projection_matrix);
    grid.Draw(cam_pos, time, offset, quad_model_matrix, view_matrix, projection_matrix,0);
    water.Draw(cam_pos, time, offset, quad_model_matrix, view_matrix, projection_matrix);
}

// transforms glfw screen coordinates into normalized OpenGL coordinates.
vec2 TransformScreenCoords(GLFWwindow* window, int x, int y) {
    // the framebuffer and the window doesn't necessarily have the same size
    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);
    return vec2(2.0f * (float)x / width - 1.0f, 1.0f - 2.0f * (float)y / height);
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

    // when the window is resized
    framebuffer.Cleanup();
    framebuffer.Init(window_width, window_height);
    noise.UpdateSize(window_width, window_height);
    water.UpdateSize(window_width, window_height);
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // escape to close window
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    float deltaXY = 0.03f; // unit of movement on height direction
    float deltaLR = 0.05f; // unit of movement on left and right cam_look
    float deltaLook = 0.01f; // unit of movement on up and down cam_look

    // look direction
    vec3 direction = cam_look - cam_pos;
    vec2 dir = vec2(direction.x,direction.z);

    // side direction
    vec3 cross_dir = cross(direction, vec3(0.0,1.0,0));
    vec2 cross_dir2 = vec2(cross_dir.x, cross_dir.z);

    // TO CHOOSE NAVIGATION MODE
    if(action == GLFW_PRESS){
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
            // need to set cam_pos to default if want to start somewhere else than current
            bezierCountPanorama = 0;
            break;
        }
    }

    // NAVIGATION CAMERA
    if(cameraStatus.x == 1){
        if(action == GLFW_REPEAT || action == GLFW_PRESS){ // make the navigation stop at the beginning
            switch(key) {

            // is able to move depending on look direction
            case GLFW_KEY_LEFT:
                offset+= vec2(-delta_offset, delta_offset)*cross_dir2;
                break;

            case GLFW_KEY_RIGHT:
                offset-= vec2(-delta_offset, delta_offset)*cross_dir2;
                break;

            case GLFW_KEY_DOWN:
                if(cam_pos.y - delta*direction.y > lake_level &&
                        cam_pos.y - delta*direction.y < sky_limit){ // avoid going under lake level
                    offset+= vec2(-delta_offset, delta_offset)*dir;
                    cam_pos.y -= delta*direction.y;
                    cam_look.y -= delta*direction.y;
                }
                break;

            case GLFW_KEY_UP:
                if(cam_pos.y + delta*direction.y > lake_level &&
                        cam_pos.y + delta*direction.y < sky_limit){ // avoid going under lake level
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
                if(std::abs(cam_look.y-cam_pos.y+deltaLook) < 0.2f) cam_look += vec3(0,deltaLook,0); // block at a certain level
                break;

            case 83: // S
                if(std::abs(cam_look.y-cam_pos.y-deltaLook) < 0.2f) cam_look -= vec3(0,deltaLook,0); // block at a certain level
                break;

            case 90: // Y
                if(cam_look.y - deltaXY > lake_level) { // avoid going under lake level
                    cam_look -= vec3(0,deltaXY,0);
                    cam_pos -= vec3(0,deltaXY,0);
                }
                break;

            case 88: // X
                if(cam_look.y + deltaXY < sky_limit){ // avoid going too high
                    cam_look += vec3(0,deltaXY,0);
                    cam_pos += vec3(0,deltaXY,0);
                }
                break;
            case 81: // Q
                if(delta_offset > 0.08) { // avoid to be negative, otherwise will go backwards
                    delta_offset -= 0.05f;
                }
                break;

            case 69: // E
                delta_offset += 0.05f;
                break;
            default:
                break;
            }
        }
    }

    if(cameraStatus.z==1){ // PANORAMA
        switch(key){
        case 90: // Y
            stopPanorama = 1;
            break;

        case 88: // X
            stopPanorama = 0;
            break;

        case 81: // Q
            // slower
            break;
        case 69: // E
            // faster
            break;
        }
    }

    // FPS CAMERA
    if(cameraStatus.y == 1){
        if(action == GLFW_PRESS /*|| action == GLFW_REPEAT*/){
            b0_fps = vec3(offset.x,0,offset.y);

            // variation of speed
            float v1 = 0.72f;
            float v2 = 1-v1;
            //if(action == GLFW_REPEAT) {
            switch(key){

            case GLFW_KEY_LEFT:
                b1_fps = b0_fps - vec3(delta_fps*v1,0,-delta_fps*v1)*cross_dir;
                b2_fps = b1_fps - vec3(delta_fps*v2,0,-delta_fps*v2)*cross_dir;
                bezierFPSStatus = 1;
                break;
            case GLFW_KEY_RIGHT:
                b1_fps = b0_fps + vec3(delta_fps*v1,0,-delta_fps*v1)*cross_dir;
                b2_fps = b1_fps + vec3(delta_fps*v2,0,-delta_fps*v2)*cross_dir;
                bezierFPSStatus = 1;
                break;
            case GLFW_KEY_DOWN:
                b1_fps = b0_fps - vec3(delta_fps*v1,0,-delta_fps*v1)*direction;
                b2_fps = b1_fps - vec3(delta_fps*v2,0,-delta_fps*v2)*direction;
                bezierFPSStatus = 1;
                break;
            case GLFW_KEY_UP:
                b1_fps = b0_fps + vec3(delta_fps*v1,0,-delta_fps*v1)*direction;
                b2_fps = b1_fps + vec3(delta_fps*v2,0,-delta_fps*v2)*direction;
                bezierFPSStatus = 1;
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

            case 81: // Q
                if(speedBezierFPS > 0.08 && speedBezierFPS < 0.9){ // to avoid go further than cameraBezierDelta and to be negative
                    speedBezierFPS -= 0.05f;
                }
                break;
            case 69: // E
                speedBezierFPS += 0.05f;
                break;
            }
            //}
        }
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
    glfwSetFramebufferSizeCallback(window, SetupProjection);

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

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
