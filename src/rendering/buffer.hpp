#pragma once
#include <vector>
#include "opengl.hpp"

class Buffer
{
    public:
        enum { ARRAY_BUFFER, ELEMENT_ARRAY_BUFFER };
        Buffer(int32_t type = ARRAY_BUFFER);
        Buffer(const std::vector<float>& data);
        Buffer(const std::vector<uint32_t>& data);
        ~Buffer();
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;
        GLuint getId() const;
        void bind() const;
        void setData(const std::vector<float>& data);
        void setData(const std::vector<uint32_t>& data);
        int32_t getElementAmount() const;
    private:
        GLuint mBufferId;
        int32_t mType;
        int32_t mElementAmount;
};
