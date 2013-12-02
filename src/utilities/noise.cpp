#include "noise.h"

Noise::Noise()
{
    setSeed(0);
}

Noise::Noise(uint32_t seed)
{
    setSeed(seed);
}

void Noise::setSeed(uint32_t seed)
{
    for(uint16_t i = 0; i < 256; i++)
    {
        mPerm[i] = i;
    }
    
    std::random_shuffle(mPerm.begin(), mPerm.begin() + 256);
    std::copy(mPerm.begin(), mPerm.begin() + 256, mPerm.begin() + 256);
}
