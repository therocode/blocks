#include "voronoinoise.h"
#include "whitenoise.h"
#include <cmath>

float VoronoiNoise::get2d(float x, float y, const uint8_t* perm) const
{
    WhiteNoise white;

    int xInt = (x > 0.0f? (int)x: (int)x - 1);
    int yInt = (y > 0.0f? (int)y: (int)y - 1);

    float minDist = 2147483647.0f;
    float xCandidate = 0.0f;
    float yCandidate = 0.0f;

    // Inside each unit cube, there is a seed point at a random position.  Go
    // through each of the nearby cubes until we find a cube with a seed point
    // that is closest to the specified position.
    for (int yCur = yInt - 2; yCur <= yInt + 2; yCur++) 
    {
        for (int xCur = xInt - 2; xCur <= xInt + 2; xCur++) 
        {

            // Calculate the position and distance to the seed point inside of
            // this unit cube.
            float xPos = xCur + white.get2d(xCur, yCur, perm);
            float yPos = yCur + white.get2d(xCur, yCur + 1000, perm); //this should be different
            float xDist = xPos - x;
            float yDist = yPos - y;
            float dist = xDist * xDist + yDist * yDist;

            if (dist < minDist) 
            {
                // This seed point is closer to any others found so far, so record
                // this seed point.
                minDist = dist;
                xCandidate = xPos;
                yCandidate = yPos;
            }
        }
    }

    // Return the calculated distance with the displacement value applied.
    return white.get2d(std::floor(xCandidate), std::floor(yCandidate), perm);
}
