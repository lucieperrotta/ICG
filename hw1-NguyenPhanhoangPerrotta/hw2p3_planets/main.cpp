// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include "quad/quad.h"

Quad sun;
Quad earth;
Quad moon;


void Init() {
    // sets background color
    glClearColor(1.0,1.0,1.0 /*white*/, 1.0 /*solid*/);

    // set images to quad
    sun.Init("sun.tga");
    earth.Init("earth.tga");
    moon.Init("moon.tga");

}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);

    float time_secs = glfwGetTime();
    const float SUN_SPEED = 0.2;
    const float SUN_ROTATION = 0.2;
    const float EARTH_SPEED = 0.5;
    const float EARTH_ROTATION = 0.5;
    const float MOON_SPEED = 0.7;
    const float DISTANCE_SUN = 0.1f;
    const float DISTANCE_EARTH = 0.2f;
    const float ACCELERATION = 2.5;

    const float ELLIPSE_A = 0.6;
    const float ELLIPSE_B = 0.4;

    // SUN
    glm::mat4 sun_translate = glm::translate( // translate the sun a bit to the right
        glm::mat4(1.0f),
        glm::vec3(0.2, 0.0f, 0.0f)
    );

    glm::mat4 sun_scale = glm::scale( // make it little
        sun_translate,
        glm::vec3(0.5f)
    );

    glm::mat4 sun_model = glm::rotate( // rotate on itself
        sun_scale,
        SUN_SPEED*time_secs*ACCELERATION,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    glm::mat4 T = IDENTITY_MATRIX;
    T[3][0] = ELLIPSE_A * cos(SUN_ROTATION*time_secs*ACCELERATION);
    T[3][1] = -ELLIPSE_B * sin(SUN_ROTATION*time_secs*ACCELERATION);

    // EARTH
    glm::mat4 earth_rotationSun = glm::rotate( // rotate around sun (ellipse)
        T,
        SUN_ROTATION,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    glm::mat4 earth_translate = glm::translate( // distance to the sun
        earth_rotationSun,
        glm::vec3(DISTANCE_SUN, 0.0f, 0.0f)
    );
    glm::mat4 earth_rotation = glm::rotate( // rotate on itself
        earth_translate,
        EARTH_SPEED*time_secs*ACCELERATION,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    glm::mat4 earth_model = glm::scale( // make it little
        earth_rotation,
        glm::vec3(0.25f)
    );

    // MOON
    glm::mat4 moon_rotationSun = glm::rotate( // rotate around sun (ellipse)
        T,
        SUN_ROTATION,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    glm::mat4 moon_translateSun = glm::translate( // distance to the sun
        moon_rotationSun,
        glm::vec3(DISTANCE_SUN, 0.0f, 0.0f)
    );
    glm::mat4 moon_rotateEarth = glm::rotate( // rotate around earth
        moon_translateSun,
        EARTH_ROTATION*time_secs*2*ACCELERATION,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    glm::mat4 moon_tranlateEarth = glm::translate( // distance to the earth
        moon_rotateEarth,
        glm::vec3(DISTANCE_EARTH, 0.0f, 0.0f)
    );
    glm::mat4 moon_rotation = glm::rotate( // rotate on itself
        moon_tranlateEarth,
        MOON_SPEED*time_secs*ACCELERATION,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    glm::mat4 moon_model = glm::scale( // make it little
        moon_rotation,
        glm::vec3(0.15f)
    );

    // compute the transformation matrices
    sun.Draw(sun_model);
    earth.Draw(earth_model);
    moon.Draw(moon_model);
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
    GLFWwindow* window = glfwCreateWindow(712, 712, "planets", NULL, NULL);
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

    sun.Cleanup();
    earth.Cleanup();
    moon.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
