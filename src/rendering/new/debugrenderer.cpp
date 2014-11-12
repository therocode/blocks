#include "debugrenderer.hpp"
#include <vector>
#include <string>
#include <iostream>

DebugRenderer::DebugRenderer()
{
    std::vector<float> vertices = {
        //-1.0f,  1.0f, -1.0f,
        //-1.0f, -1.0f, -1.0f,
        // 1.0f, -1.0f, -1.0f
             -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
               0.0f,  1.0f, 0.0f,
    };

    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);

    // Generate a buffer for the indices
    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
// in_Position was bound to attribute index 0(\"shaderAttribute\")
    std::string vertexSource = "#version 150\r\
        in  vec3 in_Position; \
      \
    void main() \
    {\
            gl_Position = vec4(in_Position.x, in_Position.y, in_Position.z, 1.0);\
    }";

    const GLchar* ptr = vertexSource.data();

    glShaderSource(vertexShader, 1, (const GLchar**)&ptr, 0);

    glCompileShader(vertexShader);

    GLint success = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE)
    {
        std::cout << "vertex shader failed compilation\n";
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
   
    std::string fragmentSource = "#version 150\rprecision highp float;\
         \
        out vec4 fragColor;\
     \
    void main()\
    { \
            fragColor = vec4(1.0,1.0,1.0,1.0);\
    }";

    ptr = fragmentSource.data();
    glShaderSource(fragmentShader, 1, (const GLchar**)&ptr, 0);

    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE)
    {
        std::cout << "fragment shader failed compilation\n";
    }

    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    /* Specify that our coordinate data is going into attribute index 0(shaderAttribute), and contains three floats per vertex */
    GLuint shaderAttribute = 0;
    glBindAttribLocation(shaderProgram, shaderAttribute, "in_Position");

    glLinkProgram(shaderProgram);

    glUseProgram(shaderProgram);
     
    /* Enable attribute index 0(shaderAttribute) as being used */
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);

    glEnableVertexAttribArray(shaderAttribute);

    glVertexAttribPointer(shaderAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);


    GLint isLinked = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinked);

    if(isLinked == GL_FALSE)
        std::cout << "not linked\n";
    std::cout << "error: " << gluErrorString(glGetError()) << "\n";
}

void DebugRenderer::queue(const Renderable& renderable)
{
}

void DebugRenderer::render()
{
    glBindVertexArray(mVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

std::type_index DebugRenderer::getRenderableType() const
{
    return std::type_index(typeid(DebugRenderable));   
}
