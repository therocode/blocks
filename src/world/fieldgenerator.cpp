#include "fieldgenerator.hpp"

void FieldGenerator::fill(const BiomeRegionCoord& coordinate, FieldGrid& grid, NoiseType type)
{
    uint32_t size = grid.getInnerSize();

    float spacing = (float)biomeRegionWidth / (float)(size - 1);

    std::cout << "will fill field size " << size << "\n";

    for(uint32_t z = 0; z < size; z++)
    {
        for(uint32_t y = 0; y < size; y++)
        {
            for(uint32_t x = 0; x < size; x++)
            {
                if(type == SIMPLEX)
                    grid.setInner({x, y, z}, mNoise.simplexOctave3D((float)x, (float)y, (float)z));
                else
                    grid.setInner({x, y, z}, mNoise.voronoi2D((float)x, (float)z)); //should be 3d when alternative exists
            }
        }
    }
}
