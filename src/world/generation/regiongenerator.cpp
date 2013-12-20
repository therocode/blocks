#include "regiongenerator.h"

IntensityMap RegionGenerator::generateHeightmap(const RegionCoord& regionCoordinate) const
{
    IntensityMap heightmap;

    for(int x = 0; x < regionWidth; x++)
    for(int y = 0; y < regionWidth; y++)
    {
        heightmap.setUnit(x, y, (float)(rand() % 1000) / 1000.f);
    }

    return heightmap;
}
