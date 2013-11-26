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
    REQUIRE(differsBy(0.0f, 0.5f) == 0.5f);
}

const float acceptableThreshold = 0.0001f;

TEST_CASE("asm_noise_3d", "[noise]")
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

    CHECK(differsBy(asm_raw_noise_3d(0.0f, 0.0f, 0.0f),  raw_noise_3d(0.0f, 0.0f, 0.0f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_3d(0.0f, 0.0f, 0.0f),  raw_noise_3d(0.0f, 0.0f, 0.0f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_3d(0.0f, -0.1f, 0.0f), raw_noise_3d(0.0f, -0.1f, 0.0f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_3d(0.0f, -0.1f, 0.0f), raw_noise_3d(0.0f, -0.1f, 0.0f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_3d(0.0f, 0.0f, -0.1f), raw_noise_3d(0.0f, 0.0f, -0.1f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_3d(0.0f, 0.0f, -0.1f), raw_noise_3d(0.0f, 0.0f, -0.1f)) < acceptableThreshold);

    float counter = 0.0f;
    float failedC = 0.0f;
    float successed = 0.0f;

    bool failed = false;
    for(int x = -10; x < 10; x++)
    {
        for(int y = -10; y < 10; y++)
        {
            for(int z = -10; z < 10; z++)
            {
                counter++;
                float xc = (float)x / 10.0f;
                float yc = (float)y / 10.0f;
                float zc = (float)z / 10.0f;

                float difference = differsBy(asm_raw_noise_3d(xc, yc, zc), raw_noise_3d(xc, yc, zc));

                if(difference > acceptableThreshold)
                {
                    failed = true;
                    failedC++;
                }
                else
                    successed++;
                CHECK(difference < acceptableThreshold);
            }
        }
    }

    INFO("3d noise: the success % is " << (successed/counter) * 100.0f << " and the failed % is " << (failedC/counter) * 100.0f << " of a total of " << counter << " runs");
    
    CHECK(failed == false);
}

TEST_CASE("asm_noise_2d", "[noise]")
{
    CHECK(differsBy(asm_raw_noise_2d(0.0f, 0.0f), raw_noise_2d(0.0f, 0.0f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_2d(100.0f, 0.0f), raw_noise_2d(100.0f, 0.0f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_2d(0.0f, 100.0f), raw_noise_2d(0.0f, 100.0f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_2d(100.0f, 100.0f), raw_noise_2d(100.0f, 100.0f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_2d(-100.0f, 0.0f), raw_noise_2d(-100.0f, 0.0f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_2d(0.0f, -100.0f), raw_noise_2d(0.0f, -100.0f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_2d(-100.0f, -100.0f), raw_noise_2d(-100.0f, -100.0f)) < acceptableThreshold);

    CHECK(differsBy(asm_raw_noise_2d(0.01f, 0.0f), raw_noise_2d(0.01f, 0.0f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_2d(0.0f, 0.01f), raw_noise_2d(0.0f, 0.01f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_2d(0.01f, 0.01f), raw_noise_2d(0.01f, 0.01f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_2d(-0.01f, 0.0f), raw_noise_2d(-0.01f, 0.0f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_2d(0.0f, -0.01f), raw_noise_2d(0.0f, -0.01f)) < acceptableThreshold);
    CHECK(differsBy(asm_raw_noise_2d(-0.01f, -0.01f), raw_noise_2d(-0.01f, -0.01f)) < acceptableThreshold);

    float counter = 0.0f;
    float failedC = 0.0f;
    float successed = 0.0f;

    bool failed = false;
    for(int x = -10; x < 10; x++)
    {
        for(int y = -10; y < 10; y++)
        {
            counter++;
            float xc = (float)x / 10.0f;
            float yc = (float)y / 10.0f;

            float difference = differsBy(asm_raw_noise_2d(xc, yc), raw_noise_2d(xc, yc));

            if(difference > acceptableThreshold)
            {
                failed = true;
                failedC++;
            }
            else
                successed++;
            CHECK(difference < acceptableThreshold);
        }
    }

    INFO("2d noise: the success % is " << (successed/counter) * 100.0f << " and the failed % is " << (failedC/counter) * 100.0f << " of a total of " << counter << " runs");
    
    CHECK(failed == false);
}
