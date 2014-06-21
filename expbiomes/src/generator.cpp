#include "generator.hpp"

void Generator::generateCoordinate(float x, float y, float& height, float& rain, float& temperature, float& biome)
{
    height = noise.simplexOctave3D((float) x / 200.0f, (float) y / 200.0f, 800.0f, 0.6, 6, 0.5f);
    height += noise.simplexOctave3D((float) x / 800.0f, (float) y / 800.0f, 1000.0f, 0.6f, 6); //big rain difference
    height = height * 1.8f;
    height = (height + 1.0f) / 2.0f;
    height = std::max(0.0f, std::min(height, 1.0f));

    rain = noise.simplexOctave3D((float) x / 200.0f, (float) y / 200.0f, 500.5, 0.6f, 6, 0.5f);
    rain += noise.simplexOctave3D((float) x / 800.0f, (float) y / 800.0f, 590.5, 0.6f, 6); //big rain difference
    rain = rain * 1.8f;
    rain = (rain + 1.0f) / 2.0f;
    rain = rain - height / 4.0f;
    rain = std::max(0.0f, std::min(rain, 1.0f));

    temperature = noise.simplexOctave3D((float) x / 200.0f, (float) y / 200.0f, 0.5, 0.6f, 6);
    temperature += noise.simplexOctave3D((float) x / 800.0f, (float) y / 800.0f, 90.5, 0.6f, 6); //big temperature difference
    temperature = temperature * 1.2f + 0.2f;
    temperature = (temperature + 1.0f) / 2.0f;
    temperature = temperature - height / 2.0f;
    temperature = std::max(0.0f, std::min(temperature, 1.0f));

    float xTurbulence = noise.simplexOctave3D((float) x / 40.0f, (float) y / 40.0f, 0.5, 0.6f, 6, 0.5f);
    float yTurbulence = noise.simplexOctave3D((float) x / 40.0f, (float) y / 40.0f, 50.5, 0.6f, 6, 0.5f);
    biome = noise.voronoi2D(x / 60.0f + xTurbulence * 0.25f, y / 60.0f + yTurbulence * 0.25f);
}
