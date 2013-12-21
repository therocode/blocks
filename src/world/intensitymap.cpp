#include "intensitymap.h"
#include "worldstd.h"

IntensityMap::IntensityMap()
{
    units = std::unique_ptr<float[]>(new float[regionWidth * regionWidth * chunkWidthx2]);
}

IntensityMap::IntensityMap(const IntensityMap& other)
{
    size_t dataSize = regionWidth * regionWidth * chunkWidthx2;
    
    units = std::unique_ptr<float[]>(new float[dataSize]);

    for(int32_t i = 0; i < dataSize; i++)
        units[i] = other.units[i];
}

IntensityMap::IntensityMap(IntensityMap&& other)
{
    std::swap(units, other.units);
}

const IntensityMap& IntensityMap::operator=(const IntensityMap& other)
{
    size_t dataSize = regionWidth * regionWidth * chunkWidthx2;
    
    units = std::unique_ptr<float[]>(new float[dataSize]);

    for(int32_t i = 0; i < dataSize; i++)
        units[i] = other.units[i];

    return *this;
}

const IntensityMap& IntensityMap::operator=(IntensityMap&& other)
{
    std::swap(units, other.units);
    return *this;
}

float IntensityMap::getUnit(size_t x, size_t y) const
{
    return units[x + y * regionWidth];
}

void IntensityMap::setUnit(size_t x, size_t y, float value)
{
    units[x + y * regionWidth] = value;
}

//void IntensityMap::toTexture(fea::Texture& texture)
//{
//    for(int x = 0; x < partSize; x++)
//    {
//        for(int y = 0; y < partSize; y++)
//        {
//            texture.setPixel(x, y, getUnit(x, y), getUnit(x, y), getUnit(x, y));
//        }
//    }
//    texture.update();
//}
