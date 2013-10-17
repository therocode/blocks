#pragma once
#include <stdint.h>
#include <random>

class Random
{
    public:
        Random();
        uint32_t randomInt();
        int32_t randomIntRange(int32_t start, int32_t end);
        float randomFloatRange(float start, float end);
        bool randomChance(float chance);
    private:
        std::random_device randomDevice;
        std::mt19937 generator;
};
