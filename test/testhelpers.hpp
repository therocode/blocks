#pragma once

const double accuracy = 0.0005;

bool closeEnough(float a, float b)
{
    return fabs(a - b) < accuracy;
}

bool closeEnough(double a, double b)
{
    return fabs(a - b) < accuracy;
}
