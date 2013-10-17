#include "random.h"

Random::Random() : generator(randomDevice())
{
}

uint32_t Random::randomInt()
{
    return std::rand();
}

int32_t Random::randomIntRange(int32_t start, int32_t end)
{
    std::uniform_int_distribution<> distribution(start, end);
    return distribution(generator);
}

float Random::randomFloatRange(float start, float end)
{
    std::uniform_real_distribution<> distribution(start, end);
    return distribution(generator);
}

bool Random::randomChance(float chance)
{
    std::uniform_real_distribution<> distribution(0.0f, 1.0f);

    return distribution(generator) < chance;
}
