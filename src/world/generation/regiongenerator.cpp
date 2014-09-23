#include "regiongenerator.hpp"
#include "../region.hpp"
#include "../../utilities/lodepng.hpp"
#include <unordered_map>

RegionGenerator::RegionGenerator()
{
}

Region RegionGenerator::generateRegion(const RegionCoord& coordinate)
{
    FloatMap height = mRegionDataGenerator.generateHeightmap(coordinate);
    Region newRegion(height, mRegionDataGenerator.generateRainfall(coordinate), mRegionDataGenerator.generateTemperature(coordinate, height), mRegionDataGenerator.generateBiomeSelector(coordinate));
    return newRegion;
}

void RegionGenerator::setSeed(int32_t seed)
{
    mRegionDataGenerator.setSeed(seed);
}
