#pragma once
#include <type_traits>
#include "templateutils.hpp"
#include "glm.hpp"
#include <fea/assert.hpp>

template<uint32_t size, uint32_t resolution, typename Type> 
class InterpolationGrid3D
{
    public:
        InterpolationGrid3D();
        InterpolationGrid3D(const Type& value);
        Type at(const glm::uvec3& location);
        void set(const glm::uvec3& location, const Type& value);
    private:
        std::array<Type, resolution * resolution * resolution> mData;
};

template<uint32_t size, uint32_t resolution, typename Type> 
InterpolationGrid3D<size, resolution, Type>::InterpolationGrid3D()
{
}

template<uint32_t size, uint32_t resolution, typename Type> 
InterpolationGrid3D<size, resolution, Type>::InterpolationGrid3D(const Type& value)
{
    mData.fill(value);
}

template<uint32_t size, uint32_t resolution, typename Type>
Type InterpolationGrid3D<size, resolution, Type>::at(const glm::uvec3& location)
{
    uint32_t index = location.x + location.y * resolution + location.z * resolution * resolution;

    FEA_ASSERT(index <= mData.size(), "arguments out of range");

    return mData[index];
}

template<uint32_t size, uint32_t resolution, typename Type>
void InterpolationGrid3D<size, resolution, Type>::set(const glm::uvec3& location, const Type& value)
{
    uint32_t index = location.x + location.y * resolution + location.z * resolution * resolution;

    FEA_ASSERT(index <= mData.size(), "arguments out of range");

    mData[index] = value;
}
