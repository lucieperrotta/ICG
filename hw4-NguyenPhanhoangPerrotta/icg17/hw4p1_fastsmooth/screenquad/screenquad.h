#pragma once
#include "icg_helper.h"

class ScreenQuad {

    private:
        GLuint vertex_array_id_;        // vertex array object
        GLuint program_id_;             // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer
        GLuint colorTexture_id_;             // texture ID
        GLuint velocityTexture_id_;             // texture ID

        float screenquad_width_;
        float screenquad_height_;

        float std_deviation;

    public:
        void Init(float screenquad_width, float screenquad_height,
                  GLuint colorTexture, GLuint velocityTexture) {

            // set screenquad size
            this->screenquad_width_ = screenquad_width;
            this->screenquad_height_ = screenquad_height;

            this->std_deviation = 3.;

            // compile the shaders
            program_id_ = icg_helper::LoadShaders("screenquad_vshader.glsl",
                                                  "screenquad_fshader.glsl");
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
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_point),
                             vertex_point, GL_STATIC_DRAW);

                // attribute
                GLuint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
                glEnableVertexAttribArray(vertex_point_id);
                glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE,
                                      ZERO_STRIDE, ZERO_BUFFER_OFFSET);
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
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_texture_coordinates),
                             vertex_texture_coordinates, GL_STATIC_DRAW);

                // attribute
                GLuint vertex_texture_coord_id = glGetAttribLocation(program_id_,
                                                                     "vtexcoord");
                glEnableVertexAttribArray(vertex_texture_coord_id);
                glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT,
                                      DONT_NORMALIZE, ZERO_STRIDE,
                                      ZERO_BUFFER_OFFSET);
            }

            // load/Assign texture
            this->colorTexture_id_ = colorTexture;
            glBindTexture(GL_TEXTURE_2D, colorTexture_id_);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            GLuint colorTex_id = glGetUniformLocation(program_id_, "colorTex");
            glUniform1i(colorTex_id, 0 /*GL_TEXTURE0*/);

            this->velocityTexture_id_ = colorTexture;
            glBindTexture(GL_TEXTURE_2D, velocityTexture_id_);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            GLuint velocityTex_id = glGetUniformLocation(program_id_, "velocityTex");
            glUniform1i(velocityTex_id, 0 /*GL_TEXTURE1*/);

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
            glDeleteTextures(1, &colorTexture_id_);
            glDeleteTextures(1, &velocityTexture_id_);
        }

        void UpdateSize(int screenquad_width, int screenquad_height) {
            this->screenquad_width_ = screenquad_width;
            this->screenquad_height_ = screenquad_height;
        }

        void changeVariance(float std_deviation) {
            // we can't go under 0.25
            this->std_deviation = fmax(0.25, this->std_deviation + std_deviation);
        }

        float* ComputeKernel() {
            int kernel_size = 1 + 6 * int(ceil(std_deviation));

            if(screenquad_width_ < kernel_size) {
                kernel_size = min(screenquad_width_, screenquad_height_);
            }

            float* kernel = new float[kernel_size];
            int x = 0;
            for(int i = 0; i < kernel_size; i++) {
                x = i-(kernel_size/2.);
                kernel[i] = exp(-(x*x)/(2.*std_deviation*std_deviation));
            }

            return kernel;
        }

        void Draw(int axis) {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // window size uniforms
            glUniform1f(glGetUniformLocation(program_id_, "tex_width"),
                        this->screenquad_width_);
            glUniform1f(glGetUniformLocation(program_id_, "tex_height"),
                        this->screenquad_height_);

            glUniform1f(glGetUniformLocation(program_id_, "axis"),
                        axis);

            // pass kernel to shader
            glUniform1fv(glGetUniformLocation(program_id_, "kernel"),
                        min(screenquad_width_, screenquad_height_), ComputeKernel());
            glUniform1f(glGetUniformLocation(program_id_, "k_length"), 1 + 6 * int(ceil(std_deviation)));

            // bind texture
            glActiveTexture(GL_TEXTURE0);

            if(axis == 0) {
                glBindTexture(GL_TEXTURE_2D, colorTexture_id_);
            }
            else {
                glBindTexture(GL_TEXTURE_2D, velocityTexture_id_);
            }


            // draw
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
