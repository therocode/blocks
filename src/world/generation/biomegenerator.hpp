#pragma once

class IntensityMap;

class BiomeGenerator
{
    public:
        BiomeGenerator(IntensityMap* h, IntensityMap* r, IntensityMap* t, IntensityMap* b, BiomeStorage& s);
        VoxelTypeMap generateTypeMap();
    private:
        IntensityMap* heightmap;
        IntensityMap* rainfall;
        IntensityMap* temperature;
        IntensityMap* biomeSelector;
        BiomeStorage& storage;
};
