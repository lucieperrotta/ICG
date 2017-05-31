#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>


class Water : public Material, public Light{

private:
    GLuint vertex_array_id_;                // vertex array object
    GLuint vertex_buffer_object_position_;  // memory buffer for positions
    GLuint vertex_buffer_object_index_;     // memory buffer for indices
    GLuint program_id_;                     // GLSL shader program ID
    GLuint framebuffer_tex_lowergrid_id_;          // texture ID
    GLuint reflection_texture_id_;          // texture ID
    GLuint num_indices_;                    // number of vertices to render
    GLuint tex_dudv_;

    int water_width_;
    int water_height_;

public:

    void UpdateSize(int water_width, int water_height) {
        this->water_width_ = water_width;
        this->water_height_ = water_height;
    }


    void Init(GLuint framebuffer_texture_id_, GLuint framebuffer_tex_lowergrid_id_, int LengthSegmentArea, float lake_level) {
        // compile the shaders.
        program_id_ = icg_helper::LoadShaders("water_vshader.glsl", "water_fshader.glsl");


        if(!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);

        // vertex one vertex array
        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        // create water grid
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
            this->framebuffer_tex_lowergrid_id_ = framebuffer_tex_lowergrid_id_;
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

    void Draw(vec3 cam_pos, float time, vec2 offset, const glm::mat4 &model = IDENTITY_MATRIX, const glm::mat4 &view = IDENTITY_MATRIX,
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
            glUniform1i(tex_id, 20);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }

        // Bind lower grid texture
        {
            glActiveTexture(GL_TEXTURE19);
            glBindTexture(GL_TEXTURE_2D, framebuffer_tex_lowergrid_id_);
            GLuint tex_id = glGetUniformLocation(program_id_, "tex_lower_grid");
            glUniform1i(tex_id, 19);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }

        // load dudv texture
        {
            int width;
            int height;
            int nb_component;
            string filename = "waterDUDV.tga";

            // set stb_image to have the same coordinates as OpenGL
            stbi_set_flip_vertically_on_load(1);
            unsigned char* water_image = stbi_load(filename.c_str(), &width, &height, &nb_component, 0);

            // if the image is null
            if(water_image == nullptr) {
                throw(string("Failed to load texture"));
            }

            glGenTextures(1, &tex_dudv_);
            glActiveTexture(GL_TEXTURE18);
            glBindTexture(GL_TEXTURE_2D, tex_dudv_);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            // check image features
            if(nb_component == 3) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, water_image);
            } else if(nb_component == 4) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, water_image);
            }

            // cleanup
            stbi_image_free(water_image);
        }
        glActiveTexture(GL_TEXTURE18);
        glBindTexture(GL_TEXTURE_2D, tex_dudv_);
        GLuint tex_dudv_id = glGetUniformLocation(program_id_, "tex_dudv");
        glUniform1i(tex_dudv_id, 18);

        // setup matrix stack - model, view, projection
        GLint model_id = glGetUniformLocation(program_id_, "model");
        glUniformMatrix4fv(model_id, ONE, DONT_TRANSPOSE, glm::value_ptr(model));
        GLint view_id = glGetUniformLocation(program_id_, "view");
        glUniformMatrix4fv(view_id, ONE, DONT_TRANSPOSE, glm::value_ptr(view));
        GLint projection_id = glGetUniformLocation(program_id_, "projection");
        glUniformMatrix4fv(projection_id, ONE, DONT_TRANSPOSE, glm::value_ptr(projection));

        // pass the current time stamp to the shader.
        glUniform1f(glGetUniformLocation(program_id_, "time"), time);

        // water get transparent with distance
        glUniform1f(glGetUniformLocation(program_id_, "cam_pos_x"), cam_pos.x);
        glUniform1f(glGetUniformLocation(program_id_, "cam_pos_z"), cam_pos.z);

        // draw
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLE_STRIP, num_indices_, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glUseProgram(0);

        //end transparency
        glDisable(GL_BLEND);
    }
};
