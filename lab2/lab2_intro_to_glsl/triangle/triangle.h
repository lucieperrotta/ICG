#pragma once
#include "icg_helper.h"

// translate, rotate, scale, perspective
#include <glm/gtc/matrix_transform.hpp>
// value_ptr
#include <glm/gtc/type_ptr.hpp>

// vertex position of the triangle
const GLfloat vpoint[] = {-1.0f, -1.0f, 0.0f,
                          1.0f, -1.0f, 0.0f,
                          0.0f,  1.0f, 0.0f,
                          +1.0f, +1.0f, 0.0f};


class Triangle {

    private:
        GLuint vertex_array_id_;        // vertex array object
        GLuint program_id_;             // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer

    public:
        void Init() {
            // LoadShaders                  <--- compile the shaders
            // glUseProgram                 <--- bind the shader (for attributes)
            //---
            // glGenVertexArrays            <--- create vertex array
            // glBindVertexArray            <--- and bind it
            //---
            // glGenBuffers                 <--- generate buffer pointer
            // glBindBuffers                <--- make it current
            // glBufferData                 <--- tell it where to find data
            //---
            // glGetAttribLocation          <--- fetch attribute ID
            // glEnableVertexAttribArray    <--- make it the current
            // glVertexAttribPointer        <--- specify layout of attribute
            //---
            // stbi_load                    <--- load image from file
            // glGenTextures                <--- create texture buffer
            // glBindTexture                <--- and bind it
            // glTexImage2D                 <--- fill the texture with image
            // stbi_image_free              <--- free the image in cpu memory
            //---
            // glGetUniformLocation         <--- fetch ID from shader
            // glUniform*(...)              <--- set the values of a uniform
            //--- To avoid resource pollution, unload resources
            // glUseProgram(0)              <--- unbind program (safety!)
            // glBindVertexArray(0)         <--- unbind array (safety!)

            // compile the shaders
            GLuint program_id = icg_helper::LoadShaders("triangle_vshader.glsl",
                                                       "triangle_fshader.glsl");

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
            GLuint vertex_buffer;
            glGenBuffers(1, &vertex_buffer);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vpoint), vpoint, GL_STATIC_DRAW);

            // position attribute, fetch Attribute ID for Vertex Positions
            GLuint position = glGetAttribLocation(program_id, "vpoint");
            glEnableVertexAttribArray(position); // enable it
            glVertexAttribPointer(position, 3, GL_FLOAT, DONT_NORMALIZE,
                                  ZERO_STRIDE, ZERO_BUFFER_OFFSET);

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
            // glDelete*(...);
        }
        
        void Draw() {
            // glUseProgram(_pid);
            // glBindVertexArray(_vao);
            //--- if there is a texture :
            // glActiveTexture          <--- enable certain tex-unit
            // glBindTexture            <--- bind it to CPU memory
            //--- if there a "changing" value to pass to the shader :
            // glUniform*(...)          <--- set the values of a uniform
            //---
            // glDraw*(...)             <--- draws the buffers
            //---
            // glBindVertexArray(0);
            // glUseProgram(0);
        }
};
