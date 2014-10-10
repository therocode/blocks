#pragma once
#include "glm.hpp"
#include <fea/assert.hpp>

template<typename Type> 
class InterpolationGrid3D
{
    public:
        InterpolationGrid3D(uint32_t size, uint32_t downSamplingFactor);
        InterpolationGrid3D(uint32_t size, uint32_t downSamplingFactor, const Type& value);
        InterpolationGrid3D(uint32_t size, uint32_t downSamplingFactor, const std::vector<Type>& values);
        Type get(const glm::uvec3& location) const;
        Type getInner(const glm::uvec3& innerLocation) const;
        void setInner(const glm::uvec3& innerLocation, const Type& value);
    private:
        inline uint32_t toIndex(const glm::uvec3& coordinates) const
        {
            return coordinates.x + coordinates.y * mInnerSize + coordinates.z * mInnerSizePow2;
        }
        uint32_t mSize;
        uint32_t mInnerSize;
        uint32_t mInnerSizePow2;
        uint32_t mDownSamplingFactor;
        std::vector<Type> mValues;
};

template<typename Type> 
InterpolationGrid3D<Type>::InterpolationGrid3D(uint32_t size, uint32_t downSamplingFactor) :
    mSize(size),
    mInnerSize(size / (std::pow(2, downSamplingFactor))),
    mInnerSizePow2(mInnerSize * mInnerSize),
    mDownSamplingFactor(downSamplingFactor)
{
    mValues.resize(mInnerSize * mInnerSize * mInnerSize);
}

template<typename Type> 
InterpolationGrid3D<Type>::InterpolationGrid3D(uint32_t size, uint32_t downSamplingFactor, const Type& value) :
    mSize(size),
    mInnerSize(size / (std::pow(2, downSamplingFactor))),
    mInnerSizePow2(mInnerSize * mInnerSize),
    mDownSamplingFactor(downSamplingFactor)
{
    mValues.resize(mInnerSize * mInnerSize * mInnerSize);
    std::fill(mValues.begin(), mValues.end(), value);
}

template<typename Type> 
InterpolationGrid3D<Type>::InterpolationGrid3D(uint32_t size, uint32_t downSamplingFactor, const std::vector<Type>& values) :
    mSize(size),
    mInnerSize(size / (std::pow(2, downSamplingFactor))),
    mInnerSizePow2(mInnerSize * mInnerSize),
    mDownSamplingFactor(downSamplingFactor)
{
    FEA_ASSERT(values.size() == mInnerSize * mInnerSize * mInnerSize, "Value vector contains " + std::to_string(values.size()) + " instead of size * size * size which is " + std::to_string(values.size()));

        mValues = values;
}

template<typename Type> 
Type InterpolationGrid3D<Type>::get(const glm::uvec3& location) const
{
    FEA_ASSERT(toIndex(location) < mValues.size(), "Index out of bounds. None of the given coordinates " + glm::to_string(location) + " should exceed " + std::to_string(mSize));
    return mValues.at(toIndex(location));
}

template<typename Type> 
Type InterpolationGrid3D<Type>::getInner(const glm::uvec3& innerLocation) const
{
    FEA_ASSERT(toIndex(innerLocation) < mValues.size(), "Index out of bounds. None of the given coordinates " + glm::to_string(innerLocation) + " should exceed " + std::to_string(mSize));
    return mValues.at(toIndex(innerLocation));
}

template<typename Type> 
void InterpolationGrid3D<Type>::setInner(const glm::uvec3& innerLocation, const Type& value)
{
    FEA_ASSERT(toIndex(innerLocation) < mValues.size(), "Index out of bounds. None of the given coordinates " + glm::to_string(innerLocation) + " should exceed " + std::to_string(mSize));
    mValues[toIndex(innerLocation)] = value;
}
