#include "regiongenerator.h"

IntensityMap RegionGenerator::generateHeightmap(const RegionCoord& regionCoordinate) const
{
    IntensityMap heightmap;

    for(int x = 0; x < regionWidth * chunkWidth; x++)
    for(int y = 0; y < regionWidth * chunkWidth; y++)
    {
        heightmap.setUnit(x, y, (float)(rand() % 1000) / 1000.f);
    }

    return heightmap;
}
