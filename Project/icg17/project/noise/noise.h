#pragma once
#include "icg_helper.h"

class Noise {

<<<<<<< HEAD
private:
    GLuint vertex_array_id_;        // vertex array object
    GLuint program_id_;             // GLSL shader program ID
    GLuint vertex_buffer_object_;   // memory buffer
    GLuint texture_id_;             // texture ID

    int noise_width_;
    int noise_height_;

public:
    void Init(int noise_width, int noise_height, GLuint texture) {

        // set noise size
        this->noise_width_ = noise_width;
        this->noise_height_ = noise_height;

        // compile the shaders
        program_id_ = icg_helper::LoadShaders("noise_vshader.glsl", "noise_fshader.glsl");
        if(!program_id_) {
            exit(EXIT_FAILURE);
=======
    private:
        GLuint vertex_array_id_;        // vertex array object
        GLuint program_id_;             // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer
        GLuint texture_id_;             // texture ID

        float noise_width_;
        float noise_height_;

    public:
        void Init(float noise_width, float noise_height, GLuint texture) {

            // set noise size
            this->noise_width_ = noise_width;
            this->noise_height_ = noise_height;

            // compile the shaders
            program_id_ = icg_helper::LoadShaders("noise_vshader.glsl", "noise_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex Array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates
            {
                const GLfloat vertex_point[] = { /*V1*/ -1.0f, -1.0f, 0.0f,
                                                 /*V2*/ +1.0f, -1.0f, 0.0f,
                                                 /*V3*/ -1.0f, +1.0f, 0.0f,
                                                 /*V4*/ +1.0f, +1.0f, 0.0f};
                // buffer
                glGenBuffers(1, &vertex_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_point), vertex_point, GL_STATIC_DRAW);

                // attribute
                GLuint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
                glEnableVertexAttribArray(vertex_point_id);
                glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // texture coordinates
            {
                const GLfloat vertex_texture_coordinates[] = { /*V1*/ 0.0f, 0.0f,
                                                               /*V2*/ 1.0f, 0.0f,
                                                               /*V3*/ 0.0f, 1.0f,
                                                               /*V4*/ 1.0f, 1.0f};
                // buffer
                glGenBuffers(1, &vertex_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_texture_coordinates), vertex_texture_coordinates, GL_STATIC_DRAW);

                // attribute
                GLuint vertex_texture_coord_id = glGetAttribLocation(program_id_, "vtexcoord");
                glEnableVertexAttribArray(vertex_texture_coord_id);
                glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // load/Assign texture
            this->texture_id_ = texture;
            glBindTexture(GL_TEXTURE_2D, texture_id_);
            GLuint tex_id = glGetUniformLocation(program_id_, "tex");
            glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);
            glBindTexture(GL_TEXTURE_2D, 0);

            // to avoid the current object being polluted
            glBindVertexArray(0);
            glUseProgram(0);
>>>>>>> parent of 58d4dcb... reset master
        }

        glUseProgram(program_id_);

        // vertex one vertex Array
        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        // vertex coordinates
        {
            const GLfloat vertex_point[] = { /*V1*/ -1.0f, -1.0f, 0.0f,
                                             /*V2*/ +1.0f, -1.0f, 0.0f,
                                             /*V3*/ -1.0f, +1.0f, 0.0f,
                                             /*V4*/ +1.0f, +1.0f, 0.0f};
            // buffer
            glGenBuffers(1, &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_point), vertex_point, GL_STATIC_DRAW);

            // attribute
            GLuint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
            glEnableVertexAttribArray(vertex_point_id);
            glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }

        // texture coordinates
        {   // SIZE OF MOUNTAINES (HOW STRECHED THEY ARE)
            float strech = 1.3f;

            const GLfloat vertex_texture_coordinates[] = { /*V1*/ 0.0f, 0.0f,
                                                           /*V2*/ strech, 0.0f,
                                                           /*V3*/ 0.0f, strech,
                                                           /*V4*/ strech, strech};
            // buffer
            glGenBuffers(1, &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_texture_coordinates), vertex_texture_coordinates, GL_STATIC_DRAW);

            // attribute
            GLuint vertex_texture_coord_id = glGetAttribLocation(program_id_, "vtexcoord");
            glEnableVertexAttribArray(vertex_texture_coord_id);
            glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }

        // load/Assign texture
        this->texture_id_ = texture;
        glBindTexture(GL_TEXTURE_2D, texture_id_);
        GLuint tex_id = glGetUniformLocation(program_id_, "tex");
        glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);
        glBindTexture(GL_TEXTURE_2D, 0);

        // to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vertex_buffer_object_);
        glDeleteProgram(program_id_);
        glDeleteVertexArrays(1, &vertex_array_id_);
        glDeleteTextures(1, &texture_id_);
    }

    void UpdateSize(int noise_width, int noise_height) {
        this->noise_width_ = noise_width;
        this->noise_height_ = noise_height;
    }

    void Draw(vec2 offset) {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        // BIG OFFSET MUFF FEEL LIKE WALKING
        glUniform1f(glGetUniformLocation(program_id_, "offset_x"), offset.x);
        glUniform1f(glGetUniformLocation(program_id_, "offset_y"),  offset.y);

        // bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id_);

        // draw
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindVertexArray(0);
        glUseProgram(0);
    }
};
