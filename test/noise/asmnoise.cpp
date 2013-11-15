#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "../catch.hpp"
#include "../../src/utilities/simplexnoise.h"
#include "../../src/utilities/asmnoise.h"

float differsBy(float a, float b)
{
    return fabs(a - b);
}

TEST_CASE("Differs by", "[differs]")
{
    REQUIRE(differsBy(0.0f, 1.0f) == 1.0f);
    REQUIRE(differsBy(0.0f, -1.0f) == 1.0f);
    REQUIRE(differsBy(1.0f, -0.0f) == 1.0f);
    REQUIRE(differsBy(5.0f, -5.0f) == 10.0f);
    REQUIRE(differsBy(5.0f, 5.0f) == 0.0f);
    REQUIRE(differsBy(0.0f, 0.5f) == 0.f);
}

const float acceptableThreshold = 0.0001f;

TEST_CASE("asm_noise", "[noise]")
{
    CHECK(differsBy(asm_raw_noise_3d(0.0f, 0.0f, 0.0f), raw_noise_3d(0.0f, 0.0f, 0.0f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_3d(100.0f, 0.0f, 0.0f), raw_noise_3d(100.0f, 0.0f, 0.0f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_3d(0.0f, 100.0f, 0.0f), raw_noise_3d(0.0f, 100.0f, 0.0f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_3d(0.0f, 0.0f, 100.0f), raw_noise_3d(0.0f, 0.0f, 100.0f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_3d(-100.0f, 0.0f, 0.0f), raw_noise_3d(-100.0f, 0.0f, 0.0f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_3d(0.0f, -100.0f, 0.0f), raw_noise_3d(0.0f, -100.0f, 0.0f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_3d(0.0f, 0.0f, -100.0f), raw_noise_3d(0.0f, 0.0f, -100.0f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_3d(100.0f, 100.0f, 100.0f), raw_noise_3d(100.0f, 100.0f, 100.0f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_3d(-100.0f, -100.0f, -100.0f), raw_noise_3d(-100.0f, -100.0f, -100.0f)) < acceptableThreshold);


    bool failed = false;
    for(int x = -10; x < 10; x++)
    {
        for(int y = -10; y < 10; y++)
        {
            for(int z = -10; z < 10; z++)
            {
                float xc = (float)x / 10.0f;
                float yc = (float)y / 10.0f;
                float zc = (float)z / 10.0f;

                float difference = differsBy(asm_raw_noise_3d(xc, yc, zc), raw_noise_3d(xc, yc, zc));

                if(difference > acceptableThreshold)
                    failed = true;
            }
        }
    }
    
    CHECK(failed == false);
}
