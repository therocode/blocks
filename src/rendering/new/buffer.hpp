#pragma once
#include "../opengl.hpp"

class Buffer
{
    public:
        Buffer();
        ~Buffer();
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;
        GLuint getId() const;
    private:
        GLuint mBufferId;
};
