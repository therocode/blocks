#pragma once
#include <vector>
#include "opengl.hpp"

class Buffer
{
    public:
        Buffer();
        ~Buffer();
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;
        GLuint getId() const;
        void bind() const;
        void setData(const std::vector<float>& data);
    private:
        GLuint mBufferId;
};
