#include "testhelpers.hpp"
#include <cmath>

bool closeEnough(float a, float b)
{
    return fabs(a - b) < accuracy;
}
