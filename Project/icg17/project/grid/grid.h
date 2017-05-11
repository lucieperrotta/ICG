#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>

#undef M_PI
#define M_PI 3.14159

struct Light {
    glm::vec3 La = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 Ld = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 Ls = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::vec3 light_pos = glm::vec3(0.0f, 10.0f, 10.0f);

    // pass light properties to the shader
    void Setup(GLuint program_id) {
        glUseProgram(program_id);

        // given in camera space
        GLuint light_pos_id = glGetUniformLocation(program_id, "light_pos");

        GLuint La_id = glGetUniformLocation(program_id, "La");
        GLuint Ld_id = glGetUniformLocation(program_id, "Ld");
        GLuint Ls_id = glGetUniformLocation(program_id, "Ls");

        glUniform3fv(light_pos_id, ONE, glm::value_ptr(light_pos));
        glUniform3fv(La_id, ONE, glm::value_ptr(La));
        glUniform3fv(Ld_id, ONE, glm::value_ptr(Ld));
        glUniform3fv(Ls_id, ONE, glm::value_ptr(Ls));
    }
};

struct Material {
    glm::vec3 ka = glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 kd = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 ks = glm::vec3(0.8f, 0.8f, 0.8f);
    float alpha = 60.0f;

    // pass material properties to the shaders
    void Setup(GLuint program_id) {
        glUseProgram(program_id);

        GLuint ka_id = glGetUniformLocation(program_id, "ka");
        GLuint kd_id = glGetUniformLocation(program_id, "kd");
        GLuint ks_id = glGetUniformLocation(program_id, "ks");
        GLuint alpha_id = glGetUniformLocation(program_id, "alpha");

        glUniform3fv(ka_id, ONE, glm::value_ptr(ka));
        glUniform3fv(kd_id, ONE, glm::value_ptr(kd));
        glUniform3fv(ks_id, ONE, glm::value_ptr(ks));
        glUniform1f(alpha_id, alpha);
    }
};


class Grid : public Material, public Light{

private:
    GLuint vertex_array_id_;                // vertex array object
    GLuint vertex_buffer_object_position_;  // memory buffer for positions
    GLuint vertex_buffer_object_index_;     // memory buffer for indices
    GLuint program_id_;                     // GLSL shader program ID

    GLuint texture_id_grid;                     // texture ID
    GLuint tex_coloring_;                  // texture for coloring

    GLuint tex_sand_;                        // texture of the sand
    GLuint tex_grass_;                       // texture of the grass
    GLuint tex_rock_;                        // texture of the rock
    GLuint tex_snow_;                        // texture of the snow
    GLuint tex_lol_;                        // texture to load all others -> make it work o/w last one is failing

    GLuint num_indices_;                    // number of vertices to render
    //GLuint MVP_id_;                         // model, view, proj matrix ID



    // BIND TEXTURE https://openclassrooms.com/courses/creez-des-programmes-en-3d-avec-opengl/les-textures-3

    void BindShader(GLuint program_id_) {
        // Bind grid texture
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id_grid);
            GLuint texture_id_grid_ = glGetUniformLocation(program_id_, "tex_grid");
            glUniform1i(texture_id_grid_, 0);
        }

        // Bind sand texture
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, tex_sand_);
            GLuint tex_sand_id = glGetUniformLocation(program_id_, "tex_sand");
            glUniform1i(tex_sand_id, 1 /*GL_TEXTURE1*/);
        }

        // Bind grass texture
        {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, tex_grass_);
            GLuint tex_grass_id = glGetUniformLocation(program_id_, "tex_grass");
            glUniform1i(tex_grass_id, 2 /*GL_TEXTURE2*/);
        }

        // Bind rock texture
        {
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, tex_rock_);
            GLuint tex_rock_id = glGetUniformLocation(program_id_, "tex_rock");
            glUniform1i(tex_rock_id, 3 /*GL_TEXTURE3*/);
        }

        // Bind snow texture
        {
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, tex_snow_);
            GLuint tex_snow_id = glGetUniformLocation(program_id_, "tex_snow");
            glUniform1i(tex_snow_id, 4 /*GL_TEXTURE4*/);
        }

        // Bind snow texture
        {
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, tex_lol_);
            GLuint tex_snow_id = glGetUniformLocation(program_id_, "tex_lol");
            glUniform1i(tex_snow_id, 5 /*GL_TEXTURE4*/);
        }

        // Cleanup
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void LoadTextures() {

        // initialize texture features
        int width;
        int height;
        int nb_component;
        string filename;

        // load sand texture

        {
            filename = "sand.tga";

            // set stb_image to have the same coordinates as OpenGL
            stbi_set_flip_vertically_on_load(1);
            unsigned char* sand_image = stbi_load(filename.c_str(), &width, &height, &nb_component, 0);

            // if the image is null
            if(sand_image == nullptr) {
                throw(string("Failed to load texture"));
                printf("OMG GIRL THERE IS A BIG FAILURE RIGHT HERE !!!\n");
            }

            glGenTextures(1, &tex_sand_);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, tex_sand_);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            // check image features
            if(nb_component == 3) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, sand_image);
            } else if(nb_component == 4) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, sand_image);
            }

            // cleanup
            stbi_image_free(sand_image);
        }


        // load grass texture
        {
            filename = "grass.tga";

            // set stb_image to have the same coordinates as OpenGL
            stbi_set_flip_vertically_on_load(1);
            unsigned char* grass_image = stbi_load(filename.c_str(), &width, &height, &nb_component, 0);

            glGenTextures(1, &tex_grass_);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, tex_grass_);

            // check image features
            if(nb_component == 3) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, grass_image);
            } else if(nb_component == 4) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, grass_image);
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            // cleanup
            stbi_image_free(grass_image);
        }

        // load rock texture
        {
            filename = "rock.tga";

            // set stb_image to have the same coordinates as OpenGL
            stbi_set_flip_vertically_on_load(1);
            unsigned char* rock_image = stbi_load(filename.c_str(), &width, &height, &nb_component, 0);

            if(rock_image == nullptr) {
                throw(string("Failed to load texture"));
                printf("OMG GIRL THERE IS A BIG FAILURE RIGHT HERE !!!\n");
            }
            glGenTextures(1, &tex_rock_);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, tex_rock_);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            // check image features
            if(nb_component == 3) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rock_image);
            } else if(nb_component == 4) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rock_image);
            }

            // cleanup
            stbi_image_free(rock_image);
        }

        // load snow texture
        {
            filename = "snow.tga";

            // set stb_image to have the same coordinates as OpenGL
            stbi_set_flip_vertically_on_load(1);
            unsigned char* snow_image = stbi_load(filename.c_str(), &width, &height, &nb_component, 0);

            glGenTextures(1, &tex_snow_);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, tex_snow_);

            // check image features
            if(nb_component == 3) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, snow_image);
            } else if(nb_component == 4) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, snow_image);
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            // cleanup
            stbi_image_free(snow_image);
        }

        // load lol texture
        {
            filename = "snow_texture.tga";

            // set stb_image to have the same coordinates as OpenGL
            stbi_set_flip_vertically_on_load(1);
            unsigned char* lol_image = stbi_load(filename.c_str(), &width, &height, &nb_component, 0);

            glGenTextures(1, &tex_lol_);
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, tex_lol_);

            // check image features
            if(nb_component == 3) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, lol_image);
            } else if(nb_component == 4) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, lol_image);
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            // cleanup
            stbi_image_free(lol_image);
        }
    }

public:
    void Init(GLuint framebuffer_texture_id_grid, float lake_level, float height_scale, int LengthSegmentArea) {
        // compile the shaders.
        program_id_ = icg_helper::LoadShaders("grid_vshader.glsl", "grid_fshader.glsl");
        if(!program_id_) {
            exit(EXIT_FAILURE);
        }
        glUseProgram(program_id_);


        // tex grid - height map
        this->texture_id_grid = framebuffer_texture_id_grid;
        // -> uniform in bind with the others


        // vertex one vertex array
        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        // vertex coordinates and indices
        {
            std::vector<GLfloat> vertices;
            std::vector<GLuint> indices;

            // nb of quad/square -> make the grid more precize
            float grid_dim = 2000;

            float c = grid_dim/5;
            int count = 0;

            // length of segment (square side)
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

        // Load the textures
        LoadTextures();

        glUniform1f(glGetUniformLocation(program_id_, "lake_level"), lake_level);
        //glUniform1f(glGetUniformLocation(program_id_, "height_scale"), height_scale);

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

        glDeleteTextures(1, &tex_sand_);
        glDeleteTextures(1, &tex_grass_);
        glDeleteTextures(1, &tex_rock_);
        glDeleteTextures(1, &tex_snow_);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Draw(float time, const glm::mat4 &model = IDENTITY_MATRIX, const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX, int upper = 1) {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        // setup variables for shading
        Material::Setup(program_id_);
        Light::Setup(program_id_);

        // setup matrix stack - model, view, projection
        GLint model_id = glGetUniformLocation(program_id_, "model");
        glUniformMatrix4fv(model_id, ONE, DONT_TRANSPOSE, glm::value_ptr(model));
        GLint view_id = glGetUniformLocation(program_id_, "view");
        glUniformMatrix4fv(view_id, ONE, DONT_TRANSPOSE, glm::value_ptr(view));
        GLint projection_id = glGetUniformLocation(program_id_, "projection");
        glUniformMatrix4fv(projection_id, ONE, DONT_TRANSPOSE, glm::value_ptr(projection));

        // uniform : which part of the mountains to draw
        glUniform1i(glGetUniformLocation(program_id_, "upper"), upper);

        BindShader(program_id_);

        // pass the current time stamp to the shader.
        glUniform1f(glGetUniformLocation(program_id_, "time"), time);

        // draw
        // You can do that by uncommenting the next line.
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // might have to change GL_TRIANGLE_STRIP to GL_TRIANGLES.
        glDrawElements(GL_TRIANGLE_STRIP, num_indices_, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glUseProgram(0);
    }
};
