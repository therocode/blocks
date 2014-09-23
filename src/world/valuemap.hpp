#pragma once
#include <string>
#include <memory>
#include <fea/assert.hpp>
#include "worldstd.hpp"

template <class ValueType>
class ValueMap
{
    public:
        ValueMap(uint32_t width, uint32_t height) :
            mWidth(width),
            mHeight(height)
        {
            units = std::unique_ptr<ValueType[]>(new ValueType[width * height]);
        }

        ValueMap(const ValueMap& other) :
            mWidth(other.mWidth),
            mHeight(other.mHeight)
        {
            size_t dataSize = mWidth * mHeight;

            units = std::unique_ptr<ValueType[]>(new ValueType[dataSize]);

            for(int32_t i = 0; i < dataSize; i++)
                units[i] = other.units[i];
        }

        ValueMap(ValueMap&& other) :
            mWidth(other.mWidth),
            mHeight(other.mHeight)
        {
            std::swap(units, other.units);
        }

        const ValueMap& operator=(const ValueMap& other)
        {
            mWidth = other.mWidth;
            mHeight = other.mHeight;

            size_t dataSize = mWidth * mHeight;

            units = std::unique_ptr<ValueType[]>(new ValueType[dataSize]);

            for(int32_t i = 0; i < dataSize; i++)
                units[i] = other.units[i];

            return *this;
        }

        const ValueMap& operator=(ValueMap&& other)
        {
            mWidth = other.mWidth;
            mHeight = other.mHeight;
            std::swap(units, other.units);
            return *this;
        }

        ValueType getUnit(uint32_t x, uint32_t y) const
        {
            FEA_ASSERT(x < mWidth && y < mHeight, "Coordinates must be within x range 0.." + std::to_string(mWidth) + " and y range 0.." + std::to_string(mHeight) + " but the given coordinate was " + std::to_string(x) + ", " + std::to_string(y));
            return units[x + y * mWidth];
        }

        void setUnit(uint32_t x, uint32_t y, ValueType value)
        {
            FEA_ASSERT(x < mWidth && y < mHeight, "Coordinates must be within x range 0.." + std::to_string(mWidth) + " and y range 0.." + std::to_string(mHeight) + " but the given coordinate was " + std::to_string(x) + ", " + std::to_string(y));
            units[x + y * mWidth] = value;
        }
        //void toTexture(fea::Texture& texture);
        uint32_t mWidth;
        uint32_t mHeight;
    private:
        std::unique_ptr<ValueType[]> units;
};

using FloatMap = ValueMap<float>;
