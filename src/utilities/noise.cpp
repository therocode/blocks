#include "noise.h"
#include "simplexnoise.h"

#ifdef NOISE_ASM
#include "asmnoise.h"
#endif

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
    mRandomEngine.seed(seed);
    for(uint16_t i = 0; i < 256; i++)
    {
        mPerm[i] = i;
    }

    std::shuffle(mPerm.begin(), mPerm.begin() + 256, mRandomEngine);
    std::copy(mPerm.begin(), mPerm.begin() + 256, mPerm.begin() + 256);
}

float Noise::white2D(float x, float y) const
{
    return mWhiteNoise.get2d(x, y, mPerm.data());
}

float Noise::simplex2D(float x, float y) const
{
#ifndef NOISE_ASM
    return raw_noise_2d(x, y, mPerm.data());
#else
	return asm_raw_noise_2d(x, y, mPerm.data());
#endif
}

float Noise::simplex3D(float x, float y, float z) const
{
#ifndef NOISE_ASM
    return raw_noise_3d(x, y, z, mPerm.data());
#else
	return asm_raw_noise_3d(x, y, z, mPerm.data());
#endif
}

// 2D Multi-octave Simplex noise.
//
// For each octave, a higher frequency/lower amplitude function will be added to the original.
// The higher the persistence [0-1], the more of each succeeding octave will be added.
float Noise::simplexOctave2D(float x, float y, float scaleFactor, uint32_t octaves, float persistence) const
{
    float total = 0.0f;
    float frequency = scaleFactor;
    float amplitude = 1.0f;

    // We have to keep track of the largest possible amplitude,
    // because each octave adds more, and we need a value in [-1, 1].
    float maxAmplitude = 0.0f;

    for(int i=0; i < octaves; i++)
    {
        total += simplex2D(x * frequency, y * frequency) * amplitude;

        frequency *= 2.0f;
        maxAmplitude += amplitude;
        amplitude *= persistence;
    }

    return total / maxAmplitude;
}

// 3D Multi-octave Simplex noise.
//
// For each octave, a higher frequency/lower amplitude function will be added to the original.
// The higher the persistence [0-1], the more of each succeeding octave will be added.
float Noise::simplexOctave3D(float x, float y, float z, float scaleFactor, uint32_t octaves, float persistence) const
{
    float total = 0.0f;
    float frequency = scaleFactor;
    float amplitude = 1.0f;

    // We have to keep track of the largest possible amplitude,
    // because each octave adds more, and we need a value in [-1, 1].
    float maxAmplitude = 0.0f;

    for(uint32_t i = 0; i < octaves; i++)
    {
        total += simplex3D(x * frequency, y * frequency, z * frequency) * amplitude;

        frequency *= 2.0f;
        maxAmplitude += amplitude;
        amplitude *= persistence;
    }

    return total / maxAmplitude;
}

const uint8_t* Noise::getHaxxiPointer() const
{
    return mPerm.data();
}

float Noise::voronoi2D(float x, float y) const
{
    return mVoronoiNosie.get2d(x, y, mPerm.data());
}
