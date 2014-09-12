#pragma once
#include <string>
#include <memory>
#include "worldstd.hpp"

template <class ValueType>
class ValueMap
{
    public:
        ValueMap()
        {
            units = std::unique_ptr<ValueType[]>(new ValueType[regionVoxelWidthx2]);
        }
        ValueMap(const ValueMap& other)
        {
            size_t dataSize = regionVoxelWidthx2;

            units = std::unique_ptr<ValueType[]>(new ValueType[dataSize]);

            for(int32_t i = 0; i < dataSize; i++)
                units[i] = other.units[i];
        }
        ValueMap(ValueMap&& other)
        {
            std::swap(units, other.units);
        }
        const ValueMap& operator=(const ValueMap& other)
        {
            size_t dataSize = regionVoxelWidthx2;

            units = std::unique_ptr<ValueType[]>(new ValueType[dataSize]);

            for(int32_t i = 0; i < dataSize; i++)
                units[i] = other.units[i];

            return *this;
        }
        const ValueMap& operator=(ValueMap&& other)
        {
            std::swap(units, other.units);
            return *this;
        }
        ValueType getUnit(size_t x, size_t y) const
        {
            return units[x + y * regionVoxelWidth];
        }
        void setUnit(size_t x, size_t y, ValueType value)
        {
            units[x + y * regionVoxelWidth] = value;
        }
        //void toTexture(fea::Texture& texture);
    private:
        std::unique_ptr<ValueType[]> units;
};