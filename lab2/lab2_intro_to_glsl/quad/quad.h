#pragma once
#include "icg_helper.h"

class Quad {

private:
    GLuint vertex_array_id_;        // vertex array object
    GLuint program_id_;             // GLSL shader program ID
    GLuint vertex_buffer_object_;   // memory buffer
    GLuint texture_id_;             // texture ID

public:
    void Init() {

        const GLfloat vertex_texture_coordinates[] = {
            /*V1*/ 0.0f, 0.0f,
            /*V2*/ 1.0f, 0.0f,
            /*V3*/ 0.0f, 1.0f,
            /*V4*/ 1.0f, 1.0f
        };

        // compile the shaders
        GLuint program_id = icg_helper::LoadShaders("quad_vshader.glsl",
                                                    "quad_fshader.glsl");

        if(!program_id) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id);

        // Vertex Array
        GLuint vertex_array_id;
        glGenVertexArrays(1, &vertex_array_id);
        check_error_gl(); // Error check

        glBindVertexArray(vertex_array_id);
        check_error_gl(); // Error check


        // Vertex Buffer
        GLuint vertex_buffer_object_;
        glGenBuffers(1, &vertex_buffer_object_);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_texture_coordinates),
                     vertex_texture_coordinates, GL_STATIC_DRAW);

        // attribute
        GLuint vertex_texture_coord_id = glGetAttribLocation(program_id_, "vtexcoord");
        glEnableVertexAttribArray(vertex_texture_coord_id);
        glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT,
                              DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);

        // Uniform matrix for triangle modification
        glm::mat4 M = glm::mat4(1.0f);
        // Id matrix init
        M[0][0] = 1;
        M[1][1] = 1;
        M[2][2] = 1;
        M[3][3] = 1;

        // Edits
        M[3][1] = 0.3;

        GLuint M_id = glGetUniformLocation(program_id, "M");
        glUniformMatrix4fv(M_id, 1, GL_FALSE, glm::value_ptr(M));
    }

    void Cleanup() {
        //...
    }

    void Draw() {
        //...
    }
};
