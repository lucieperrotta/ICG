#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>


class Water : public Material, public Light{

private:
    GLuint vertex_array_id_;                // vertex array object
    GLuint vertex_buffer_object_position_;  // memory buffer for positions
    GLuint vertex_buffer_object_index_;     // memory buffer for indices
    GLuint program_id_;                     // GLSL shader program ID
    GLuint reflection_texture_id_;          // texture ID
    GLuint num_indices_;                    // number of vertices to render

    int water_width_;
    int water_height_;

public:

    void UpdateSize(int water_width, int water_height) {
        this->water_width_ = water_width;
        this->water_height_ = water_height;
    }


    void Init(GLuint framebuffer_texture_id_, int LengthSegmentArea, float lake_level) {
        // compile the shaders.
        program_id_ = icg_helper::LoadShaders("water_vshader.glsl", "water_fshader.glsl");


        if(!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);

        // vertex one vertex array
        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        // vertex coordinates and indices
        {
            std::vector<GLfloat> vertices;
            std::vector<GLuint> indices;

            float grid_dim = 2000;

            float c = grid_dim/5;
            int count = 0;
            float d = LengthSegmentArea/c;

            for(int j = 0; j < c; j++) { // y
                for(int i = 0; i < c; i++) { // x
                    float x = i*d;
                    float y = j*d;
                    float x1 = (i + 1.0f)*d;
                    float y1 = (j + 1.0f)*d;

                    // vertex position of the triangles.
                    vertices.push_back(-1.0f + x); vertices.push_back(1.0f - y);
                    vertices.push_back(-1.0f + x1); vertices.push_back(1.0f - y);
                    vertices.push_back(-1.0f + x1); vertices.push_back(1.0f - y1);
                    vertices.push_back(-1.0f + x); vertices.push_back(1.0f - y1);

                    // and indices.
                    indices.push_back(0 + 4*count);
                    indices.push_back(1 + 4*count);
                    indices.push_back(3 + 4*count);
                    indices.push_back(2 + 4*count);

                    if(i==c-1) { // to avoid having intermediate lines between rows
                        indices.push_back(2 + 4*count);
                        indices.push_back(0 + 4*(count+1));
                    }

                    count++;
                }

            }

            num_indices_ = indices.size();

            // position buffer
            glGenBuffers(1, &vertex_buffer_object_position_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position_);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

            // vertex indices
            glGenBuffers(1, &vertex_buffer_object_index_);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

            // position shader attribute
            GLuint loc_position = glGetAttribLocation(program_id_, "position");
            glEnableVertexAttribArray(loc_position);
            glVertexAttribPointer(loc_position, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);

        }

        // load texture
        {
            reflection_texture_id_ = framebuffer_texture_id_;

            // cleanup
            glBindTexture(GL_TEXTURE_2D, 0);
            //stbi_image_free(image);
        }

        glUniform1f(glGetUniformLocation(program_id_, "lake_level"), lake_level);

        // to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vertex_buffer_object_position_);
        glDeleteBuffers(1, &vertex_buffer_object_index_);
        glDeleteVertexArrays(1, &vertex_array_id_);
        glDeleteProgram(program_id_);
        glDeleteTextures(1, &reflection_texture_id_);
    }

    void Draw(float time, vec2 offset, const glm::mat4 &model = IDENTITY_MATRIX, const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX) {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        // transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Bind reflection texture
        {
            glActiveTexture(GL_TEXTURE20);
            glBindTexture(GL_TEXTURE_2D, reflection_texture_id_);
            GLuint tex_id = glGetUniformLocation(program_id_, "tex_water");
            glUniform1i(tex_id, 20 /*GL_TEXTURE0*/);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }

        // setup matrix stack - model, view, projection
        GLint model_id = glGetUniformLocation(program_id_, "model");
        glUniformMatrix4fv(model_id, ONE, DONT_TRANSPOSE, glm::value_ptr(model));
        GLint view_id = glGetUniformLocation(program_id_, "view");
        glUniformMatrix4fv(view_id, ONE, DONT_TRANSPOSE, glm::value_ptr(view));
        GLint projection_id = glGetUniformLocation(program_id_, "projection");
        glUniformMatrix4fv(projection_id, ONE, DONT_TRANSPOSE, glm::value_ptr(projection));

        // pass the current time stamp to the shader.
        glUniform1f(glGetUniformLocation(program_id_, "time"), time);
        glUniform1f(glGetUniformLocation(program_id_, "offsetX"), offset.x);
        glUniform1f(glGetUniformLocation(program_id_, "offsetY"), offset.y);
        glUniform1f(glGetUniformLocation(program_id_, "water_width"), this->water_width_);
        glUniform1f(glGetUniformLocation(program_id_, "water_height"), this->water_height_);

        // draw
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLE_STRIP, num_indices_, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glUseProgram(0);

        //end transparency
        glDisable(GL_BLEND);
    }
};
