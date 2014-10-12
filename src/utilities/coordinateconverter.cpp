#include "coordinateconverter.hpp"

int64_t wrapInt(int64_t kX, int64_t const kLowerBound, int64_t const kUpperBound)
{
    int64_t range_size = kUpperBound - kLowerBound + 1;

    if (kX < kLowerBound)
        kX += range_size * ((kLowerBound - kX) / range_size + 1);

    return kLowerBound + (kX - kLowerBound) % range_size;
}
