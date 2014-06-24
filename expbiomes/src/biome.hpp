#pragma once
#include <string>

struct Range
{
    Range(float mi, float ma) : min(mi), max(ma) {};
    bool isWithin(float v) 
    {
        return v >= min && v <= max;
    }
    float min;
    float max;
};

struct Biome
{
    Biome(const std::string& n, float red, float green, float blue, Range temp, Range rain, Range height) : name(n), r(red), g(green), b(blue), temperatureRange(temp), rainfallRange(rain), heightRange(height) {};
    std::string name;
    float r;
    float g;
    float b;
    Range temperatureRange;
    Range rainfallRange;
    Range heightRange;
};

