#include "generator.hpp"

float factor = 1.0f;

Generator::Generator() :
    noise0(0),
    noise1(1),
    noise2(2),
    noise3(3),
    noise4(4),
    noise5(5),
    noise6(6),
    noise7(7)
{
}

void Generator::generateCoordinate(float x, float y, float& height, float& rain, float& temperature, float& biome)
{
    float simplexAX = (x / 200.0f) * factor;
    float simplexAY = (y / 200.0f) * factor;
    float simplexBX = (x / 800.0f) * factor;
    float simplexBY = (y / 800.0f) * factor;
    float simplexCX = (x / 40.0f ) * factor;
    float simplexCY = (y / 40.0f ) * factor;
    
    height =  noise0.simplexOctave2D(simplexAX, simplexAY, 0.6, 6, 0.5f);
    height += noise1.simplexOctave2D(simplexBX, simplexBY, 0.6f, 6); //big rain difference
    height = height * 1.8f;
    height = (height + 1.0f) / 2.0f;
    height = std::max(0.0f, std::min(height, 1.0f));

    rain =  noise2.simplexOctave2D(simplexAX, simplexAY, 0.6f, 6, 0.5f);
    rain += noise3.simplexOctave2D(simplexBX, simplexBY, 0.6f, 6); //big rain difference
    rain = rain * 1.8f;
    rain = (rain + 1.0f) / 2.0f;
    rain = rain - height / 4.0f;
    rain = std::max(0.0f, std::min(rain, 1.0f));

    temperature =  noise4.simplexOctave2D(simplexAX, simplexAY, 0.6f, 6);
    temperature += noise5.simplexOctave2D(simplexBX, simplexBY, 0.6f, 6); //big temperature difference
    temperature = temperature * 1.2f + 0.2f;
    temperature = (temperature + 1.0f) / 2.0f;
    temperature = temperature - height / 2.0f;
    temperature = std::max(0.0f, std::min(temperature, 1.0f));

    float xTurbulence = noise6.simplexOctave2D(simplexCX,  simplexCY, 0.6f, 6, 0.5f);
    float yTurbulence = noise7.simplexOctave2D(simplexCX,  simplexCY, 0.6f, 6, 0.5f);
    biome = noise0.voronoi2D((x / 60.0f + xTurbulence * 0.25f) * factor, (y / 60.0f + yTurbulence * 0.25f) * factor);
}
