#pragma once
#include "opengl.hpp"

class VAO
{
    public:
        VAO();
        ~VAO();
        VAO(const VAO&) = delete;
        VAO& operator=(const VAO&) = delete;
        GLuint getId() const;
        void bind();
        void unbind();
    private:
        GLuint mVAOId;
};
