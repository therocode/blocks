#include "regiongenerator.h"
#include "utilities/noise.h"
#include <iostream>

IntensityMap RegionGenerator::generateHeightmap(const RegionCoord& regionCoordinate) const
{
    IntensityMap heightmap;
    Noise simplex(globalSeed + 3);
    Noise simplex2(globalSeed + 17);

    glm::vec3 regionPos = regionToWorld(regionCoordinate);

    for(size_t y = 0; y < regionVoxelWidth; y++)
    for(size_t x = 0; x < regionVoxelWidth; x++)
    {
        //float value = raw_noise_3d((float) x / 800.0f, (float) y / 800.0f, 10.5);
        float value = simplex.simplexOctave2D((float) (x + regionPos.x) / 800.0f, (float) (y + regionPos.z) / 800.0f, 0.6f, 6);
        value +=      simplex2.simplexOctave2D((float) (x + regionPos.x) / 3800.0f, (float) (y + regionPos.z) / 3800.0f, 0.6f, 6); //big height difference
        //value = simplex.white2D((float) x / 2.0f, (float) y / 2.0f);

        //float value = (perlin.GetValue((float) x / 800.0f, (float) y / 800.0f, 1000.5));
        //std::cout << "value: " << value << "\n";
        value = value * 1.8f;
        value = (value + 1.0f) / 2.0f;
        value = std::max(0.0f, std::min(value, 1.0f));
        heightmap.setUnit(x, y, value);
    }

    return heightmap;
}

IntensityMap RegionGenerator::generateRainfall(const RegionCoord& regionCoordinate) const
{
    IntensityMap rainmap;
    Noise simplex(globalSeed + 9);
    Noise simplex2(globalSeed + 41234);

    glm::vec3 regionPos = regionToWorld(regionCoordinate);

    for(size_t y = 0; y < regionVoxelWidth; y++)
    for(size_t x = 0; x < regionVoxelWidth; x++)
    {
        //float value = raw_noise_3d((float) x / 800.0f, (float) y / 800.0f, 10.5);
        float value = simplex.simplexOctave2D((float) (x + regionPos.x) / 800.0f, (float) (y + regionPos.z) / 800.0f, 0.6f, 6);
        value +=      simplex2.simplexOctave2D((float) (x + regionPos.x) / 3800.0f, (float) (y + regionPos.z) / 3800.0f, 0.6f, 6); //big height difference
        //value = simplex.white2D((float) x / 2.0f, (float) y / 2.0f);

        //float value = (perlin.GetValue((float) x / 800.0f, (float) y / 800.0f, 1000.5));
        //std::cout << "value: " << value << "\n";
        value = value * 1.8f;
        value = (value + 1.0f) / 2.0f;
        value = value;// - rainmap.getUnit(x, y) / 4.0f;  dunno what goes here
        value = std::max(0.0f, std::min(value, 1.0f));
        rainmap.setUnit(x, y, value);
    }

    return rainmap;
}

IntensityMap RegionGenerator::generateTemperature(const RegionCoord& regionCoordinate) const
{
    IntensityMap temperaturemap;
    Noise simplex(globalSeed + 123412);
    Noise simplex2(globalSeed + 1723);

    glm::vec3 regionPos = regionToWorld(regionCoordinate);

    for(size_t y = 0; y < regionVoxelWidth; y++)
    for(size_t x = 0; x < regionVoxelWidth; x++)
    {
        //float value = raw_noise_3d((float) x / 800.0f, (float) y / 800.0f, 10.5);
        float value = simplex.simplexOctave2D((float) (x + regionPos.x) / 800.0f, (float) (y + regionPos.z) / 800.0f, 0.6f, 6);
        //float value = simplex.simplex2D((float) (x + regionPos.x) / 80.0f, (float) (y + regionPos.z) / 80.0f);
        value +=      simplex2.simplexOctave2D((float) (x + regionPos.x) / 3800.0f, (float) (y + regionPos.z) / 3800.0f, 0.6f, 6); //big height difference
        //value = simplex.white2D((float) x / 2.0f, (float) y / 2.0f);

        //float value = (perlin.GetValue((float) x / 800.0f, (float) y / 800.0f, 1000.5));
        //std::cout << "value: " << value << "\n";
        value = value * 1.8f;
        value = (value + 1.0f) / 2.0f;
        value = value; //- temperaturemap.getUnit(x, y) / 2.0f;    heightmap goes here to affect temperature!
        value = std::max(0.0f, std::min(value, 1.0f));
        temperaturemap.setUnit(x, y, value);
    }

    return temperaturemap;
}

IntensityMap RegionGenerator::generateBiomeSelector(const RegionCoord& regionCoordinate) const
{
    IntensityMap biomSelectorMap;
    Noise simplex(globalSeed + 9123);
    Noise simplex2(globalSeed + 23423);

    glm::vec3 regionPos = regionToWorld(regionCoordinate);

    for(size_t y = 0; y < regionVoxelWidth; y++)
    for(size_t x = 0; x < regionVoxelWidth; x++)
    {
        float xTurbulence = simplex.simplexOctave2D((float) (x + regionPos.x) / 80.0f, (float) (y + regionPos.z) / 80.0f, 0.6f, 6);
        float yTurbulence = simplex2.simplexOctave2D((float) (x + regionPos.x) / 80.0f, (float) (y + regionPos.z) / 80.0f, 0.6f, 6); //big height difference

        //float value = (voronoi.GetValue(((float) x / 70.0f) + xTurbulence * 0.3f, ((float) y / 70.0f) + yTurbulence * 0.3f, 0.5) + 2.0f) / 3.7f;
        float value = simplex.voronoi2D((x + regionPos.x) / 80.0f + xTurbulence * 0.25f, (y + regionPos.z) / 80.0f + yTurbulence * 0.25f);
        biomSelectorMap.setUnit(x, y, value);
    }

    return biomSelectorMap;
}
