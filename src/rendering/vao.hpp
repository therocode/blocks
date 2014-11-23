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
        void setVertexAttribute(GLuint index, const uint32_t floatAmount, const float* data);
        void setVertexAttribute(GLuint index, const uint32_t floatAmount, const Buffer& dataBuffer);
        void setInstanceAttribute(GLuint index, const uint32_t floatAmount, const Buffer& dataBuffer, uint32_t divisor);
    private:
        std::vector<GLint> mEnabledVertexAttributes;
        GLuint mVAOId;
        bool mIsBound;
};
