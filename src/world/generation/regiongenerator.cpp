#include "regiongenerator.h"
#include "utilities/noise.h"
#include <iostream>

IntensityMap RegionGenerator::generateHeightmap(const RegionCoord& regionCoordinate) const
{
    IntensityMap heightmap;
    Noise simplex(900);
    Noise simplex2(900 + 17);

    std::cout << "region: " << glm::to_string((glm::ivec2)regionCoordinate) << "\n";

    glm::vec3 regionPos = regionToWorld(regionCoordinate);

    for(size_t y = 0; y < regionWidth * chunkWidth; y++)
    for(size_t x = 0; x < regionWidth * chunkWidth; x++)
    {
        //float value = raw_noise_3d((float) x / 200.0f, (float) y / 200.0f, 10.5);
        float value = simplex.simplexOctave2D((float) (x + regionPos.x) / 200.0f, (float) (y + regionPos.y) / 200.0f, 0.6f, 6);
        value +=      simplex2.simplexOctave2D((float) (x + regionPos.x) / 800.0f, (float) (y + regionPos.y) / 800.0f, 0.6f, 6); //big height difference
        //value = simplex.white2D((float) x / 2.0f, (float) y / 2.0f);

        //float value = (perlin.GetValue((float) x / 200.0f, (float) y / 200.0f, 1000.5));
        //std::cout << "value: " << value << "\n";
        value = value * 1.8f;
        value = (value + 1.0f) / 2.0f;
        value = std::max(0.0f, std::min(value, 1.0f));
        heightmap.setUnit(x, y, value);
    }

    return heightmap;
}
