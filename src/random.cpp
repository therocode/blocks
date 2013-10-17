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
