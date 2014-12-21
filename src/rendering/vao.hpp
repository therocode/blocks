#pragma once
#include "opengl.hpp"
#include "buffer.hpp"

class VAO
{
    public:
        VAO();
        ~VAO();
        VAO(const VAO&) = delete;
        VAO& operator=(const VAO&) = delete;
        GLuint getId() const;
        bool isBound() const;
        void bind();
        void unbind();
        void setVertexAttribute(GLuint index, const uint32_t floatAmount, const float* data, GLint type = GL_FLOAT);
        void setVertexAttribute(GLuint index, const uint32_t floatAmount, const Buffer& dataBuffer, GLint type = GL_FLOAT);
        void setInstanceAttribute(GLuint index, const uint32_t floatAmount, const Buffer& dataBuffer, uint32_t divisor, GLint type = GL_FLOAT);
    private:
        std::vector<GLint> mEnabledVertexAttributes;
        GLuint mVAOId;
        bool mIsBound;
};
