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

    /*
    Model maps from an object's local coordinate space into world space,
    view from world space to camera space,
    projection from camera to screen.
    */

    float time_secs = glfwGetTime();
    float sun_angle = 0.5;
    float distanceToSun = 1.0f;
    float distanceToEarth = 0.2f;

    // SUN

    glm::mat4 sun_viewTranslate = glm::translate(
        glm::mat4(1.0f),
        glm::vec3(0.0f, 0.0f, 0.0f)
    );
    glm::mat4 sun_viewRotation = glm::rotate( // rotate on itself
        sun_viewTranslate,
        sun_angle*time_secs,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );


    // EARTH

    glm::mat4 earth_viewTranslate = glm::translate( // distance to the sun
        glm::mat4(1.0f),
        glm::vec3(distanceToSun, 0.0f, 0.0f)
    );
    glm::mat4 earth_viewRotation = glm::rotate(
        earth_viewTranslate,
        sun_angle*time_secs,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    glm::mat4 earth_model = glm::scale( // make it little -> pas sure des coords ??
        glm::mat4(0.25f),
        glm::vec3(0.25f)
    );
    glm::mat4 earth_projection = glm::translate( // empty translate
                glm::mat4(1.0f),
                glm::vec3(0.0f, 0.0f, 0.0f)
    );
    glm::mat4 earth_projection2 = glm::rotate( // rotate projection -> rotate around sun
                earth_projection,
                sun_angle*time_secs,
                glm::vec3(0.0f, 0.0f, 1.0f)
    );

    // MOON

    glm::mat4 moon_viewTranslate = glm::translate( // distance to the sun
        glm::mat4(1.0f),
        glm::vec3(distanceToSun, 0.0f, 0.0f)
    );
    glm::mat4 moon_viewRotation = glm::rotate( // rotate on itself
        moon_viewTranslate,
        sun_angle*time_secs,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    glm::mat4 moon_model = glm::rotate( // rotate around earth
        glm::mat4(1.f),
        sun_angle*time_secs*2,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    glm::mat4 moon_model2 = glm::translate( // distance to the earth
        moon_model,
        glm::vec3(distanceToEarth, 0.0f, 0.0f)
    );
    glm::mat4 moon_model3 = glm::scale( // make it little
                  moon_model2,
                  glm::vec3(0.15f)
    );

    glm::mat4 moon_projection = glm::translate( // empty translate
                glm::mat4(1.f),
                glm::vec3(0.0f, 0.0f, 0.0f)
    );
    glm::mat4 moon_projection2 = glm::rotate( // rotate around sun
                moon_projection,
                sun_angle*time_secs,
                glm::vec3(0.0f, 0.0f, 1.0f)
    );

    // compute the transformation matrices
    sun.Draw(IDENTITY_MATRIX, sun_viewRotation);
    earth.Draw(earth_model, earth_viewRotation, earth_projection2);
    moon.Draw(moon_model3, moon_viewRotation, moon_projection2);
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
