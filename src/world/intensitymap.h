#pragma once
#include <string>
#include <memory>

class IntensityMap
{
    public:
        IntensityMap();
        IntensityMap(const IntensityMap& other);
        IntensityMap(IntensityMap&& other);
        const IntensityMap& operator=(const IntensityMap& other);
        const IntensityMap& operator=(IntensityMap&& other);
        float getUnit(size_t x, size_t y) const;
        void setUnit(size_t x, size_t y, float value);
        //void toTexture(fea::Texture& texture);
    private:
        std::unique_ptr<float[]> units;
};
