#include "fieldgenerator.hpp"

void FieldGenerator::fill(const BiomeRegionCoord& coordinate, FieldGrid& grid, NoiseType type)
{
    uint32_t size = grid.getInnerSize();

    float spacing = (float)biomeRegionWidth / (float)(size - 1);

    for(uint32_t z = 0; z < size; z++)
    {
        for(uint32_t y = 0; y < size; y++)
        {
            for(uint32_t x = 0; x < size; x++)
            {
                if(type == SIMPLEX)
                {
                    glm::vec3 noiseCoord = (glm::vec3)coordinate * (float)biomeRegionWidth + glm::vec3(x, y, z) * spacing;

                    grid.setInner({x, y, z}, (mNoise.simplexOctave2D(noiseCoord.x / 800.0f, noiseCoord.z / 800.0f) + 1.0f) / 2.0f);
                    if(z == 8 && y == 8)
                    std::cout << "jeje: " << glm::to_string(glm::uvec3(x, y, z)) << " " << (mNoise.simplexOctave3D((float)x, (float)y, (float)z) + 1.0f) / 2.0f << "\n";
                }
                else
                {
                    grid.setInner({x, y, z}, mNoise.voronoi2D((float)x, (float)z)); //should be 3d when alternative exists
                }
            }
        }
    }
}
