#include "vertexattribute.hpp"

VertexAttribute::VertexAttribute(const std::string& name, const uint32_t fAmount, const float* d) : mName(name), mFloatAmount(fAmount), mData(d)
{
}
