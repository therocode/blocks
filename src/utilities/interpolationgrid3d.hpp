#pragma once
#include "glm.hpp"
#include <fea/assert.hpp>
#include <functional>
#include <array>

//interpolator arguments follows the standard: x + y * width + z * width*width 
//where width = 2. 

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
        void setInterpolator(std::function<Type(const std::array<Type, 8>&, const glm::vec3& point)> interpolator);
    private:
        inline uint32_t toIndex(const glm::uvec3& coordinates) const
        {
            return coordinates.x + coordinates.y * mSize + coordinates.z * mSizePow2;
        }
        inline uint32_t toInnerIndex(const glm::uvec3& coordinates) const
        {
            return coordinates.x + coordinates.y * mInnerSize + coordinates.z * mInnerSizePow2;
        }
        uint32_t mDownSamplingFactor;
        uint32_t mDownSamplingPow;
        uint32_t mSize;
        uint32_t mSizePow2;
        uint32_t mInnerSize;
        uint32_t mInnerSizePow2;
        std::vector<Type> mValues;
        std::function<Type(const std::array<Type, 8>&, const glm::vec3& point)> mInterpolator;
};

template<typename Type> 
InterpolationGrid3D<Type>::InterpolationGrid3D(uint32_t size, uint32_t downSamplingFactor) :
    mDownSamplingFactor(downSamplingFactor),
    mDownSamplingPow(std::pow(2, downSamplingFactor)),
    mSize(size),
    mSizePow2(mSize * mSize),
    mInnerSize(size / mDownSamplingPow + (downSamplingFactor ? 1 : 0)),
    mInnerSizePow2(mInnerSize * mInnerSize)
{
    mValues.resize(mInnerSize * mInnerSize * mInnerSize);
}

template<typename Type> 
InterpolationGrid3D<Type>::InterpolationGrid3D(uint32_t size, uint32_t downSamplingFactor, const Type& value) :
    mDownSamplingFactor(downSamplingFactor),
    mDownSamplingPow(std::pow(2, downSamplingFactor)),
    mSize(size),
    mSizePow2(mSize * mSize),
    mInnerSize(size / mDownSamplingPow + (downSamplingFactor ? 1 : 0)),
    mInnerSizePow2(mInnerSize * mInnerSize)
{
    mValues.resize(mInnerSize * mInnerSize * mInnerSize);
    std::fill(mValues.begin(), mValues.end(), value);
}

template<typename Type> 
InterpolationGrid3D<Type>::InterpolationGrid3D(uint32_t size, uint32_t downSamplingFactor, const std::vector<Type>& values) :
    mDownSamplingFactor(downSamplingFactor),
    mDownSamplingPow(std::pow(2, downSamplingFactor)),
    mSize(size),
    mSizePow2(mSize * mSize),
    mInnerSize(size / mDownSamplingPow + (downSamplingFactor ? 1 : 0)),
    mInnerSizePow2(mInnerSize * mInnerSize)
{
    FEA_ASSERT(values.size() == mInnerSize * mInnerSize * mInnerSize, "Value vector contains " + std::to_string(values.size()) + " instead of size * size * size which is " + std::to_string(values.size()));

        mValues = values;
}

template<typename Type> 
Type InterpolationGrid3D<Type>::get(const glm::uvec3& location) const
{
    if(mDownSamplingFactor == 0)
    {
        FEA_ASSERT(toIndex(location) < mSize * mSize * mSize, "Index out of bounds. None of the given coordinates " + glm::to_string(location) + " should exceed " + std::to_string(mSize -1));
        return mValues.at(toIndex(location));
    }
    else
    {
        FEA_ASSERT(toIndex(location) < (mSize * mSize * mSize), "Index out of bounds. None of the given coordinates " + glm::to_string(location) + " should exceed " + std::to_string(mSize -1));

        std::array<Type, 8> interpolationPoints;

        glm::uvec3 innerPoint = location / mDownSamplingPow; 
        glm::vec3 point = glm::fract((glm::vec3)location / (float)mDownSamplingPow);

        if(glm::length(point) < 0.000005f)
            return mValues.at(toInnerIndex(innerPoint));

        interpolationPoints[0 + 0 + 0] = mValues.at(toInnerIndex(innerPoint) + 0 + 0          + 0             );
        interpolationPoints[1 + 0 + 0] = mValues.at(toInnerIndex(innerPoint) + 1 + 0          + 0             );
        interpolationPoints[0 + 2 + 0] = mValues.at(toInnerIndex(innerPoint) + 0 + mInnerSize + 0             );
        interpolationPoints[1 + 2 + 0] = mValues.at(toInnerIndex(innerPoint) + 1 + mInnerSize + 0             );
        interpolationPoints[0 + 0 + 4] = mValues.at(toInnerIndex(innerPoint) + 0 + 0          + mInnerSizePow2);
        interpolationPoints[1 + 0 + 4] = mValues.at(toInnerIndex(innerPoint) + 1 + 0          + mInnerSizePow2);
        interpolationPoints[0 + 2 + 4] = mValues.at(toInnerIndex(innerPoint) + 0 + mInnerSize + mInnerSizePow2);
        interpolationPoints[1 + 2 + 4] = mValues.at(toInnerIndex(innerPoint) + 1 + mInnerSize + mInnerSizePow2);

        FEA_ASSERT(mInterpolator, "No interpolator given!");

        return mInterpolator(interpolationPoints, point);
    }
}

template<typename Type> 
Type InterpolationGrid3D<Type>::getInner(const glm::uvec3& innerLocation) const
{
    FEA_ASSERT(toInnerIndex(innerLocation) < mValues.size(), "Index out of bounds. None of the given coordinates " + glm::to_string(innerLocation) + " should exceed " + std::to_string(mInnerSize - 1));
    return mValues.at(toInnerIndex(innerLocation));
}

template<typename Type> 
void InterpolationGrid3D<Type>::setInner(const glm::uvec3& innerLocation, const Type& value)
{
    FEA_ASSERT(toInnerIndex(innerLocation) < mValues.size(), "Index out of bounds. None of the given coordinates " + glm::to_string(innerLocation) + " should exceed " + std::to_string(mInnerSize - 1));
    mValues[toInnerIndex(innerLocation)] = value;
}

template<typename Type>
void InterpolationGrid3D<Type>::setInterpolator(std::function<Type(const std::array<Type, 8>&, const glm::vec3& point)> interpolator)
{
    mInterpolator = interpolator;
}
