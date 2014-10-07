#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include "world/worldconstants.hpp"
#include "world/region.hpp"
#include "world/valuemap.hpp"

float accuracy = 0.0004f;

#define isCloseEnough(A, B) \
   ( \
    fabs((A) - (B)) <= accuracy \
   )

SCENARIO("Fragments can be gotten from regions","[world]")
{
    GIVEN("A region filled with data")
    {   
        ValueMap<float> valueMap(100, 100);

        for(uint32_t y = 0; y < 100; y++)
        {
            for(uint32_t x = 0; x < 100; x++)
            {
                valueMap.setUnit(x, y, x + y * 100.0f);
            }
        }

        Region region(valueMap, valueMap, valueMap, valueMap);

        WHEN("A fragment of the region is retrieved")
        {   
            RegionDataFragment fragment = region.getDataFragment({40, 40}, {4, 4});

            THEN("The values gotten back match the expected region")
            {   
                CAPTURE(fragment.getHeightData().getUnit(0, 0));
                CHECK(isCloseEnough(fragment.getHeightData().getUnit(0, 0), 4040.0f));
                CAPTURE(fragment.getHeightData().getUnit(1, 0));
                CHECK(isCloseEnough(fragment.getHeightData().getUnit(1, 0), 4041.0f));
                CAPTURE(fragment.getHeightData().getUnit(2, 0));
                CHECK(isCloseEnough(fragment.getHeightData().getUnit(2, 0), 4042.0f));
                CAPTURE(fragment.getHeightData().getUnit(3, 0));
                CHECK(isCloseEnough(fragment.getHeightData().getUnit(3, 0), 4043.0f));
                CAPTURE(fragment.getHeightData().getUnit(0, 1));
                CHECK(isCloseEnough(fragment.getHeightData().getUnit(0, 1), 4140.0f));
                CAPTURE(fragment.getHeightData().getUnit(1, 1));
                CHECK(isCloseEnough(fragment.getHeightData().getUnit(1, 1), 4141.0f));
                CAPTURE(fragment.getHeightData().getUnit(2, 1));
                CHECK(isCloseEnough(fragment.getHeightData().getUnit(2, 1), 4142.0f));
                CAPTURE(fragment.getHeightData().getUnit(3, 1));
                CHECK(isCloseEnough(fragment.getHeightData().getUnit(3, 1), 4143.0f));
                CAPTURE(fragment.getHeightData().getUnit(0, 2));
                CHECK(isCloseEnough(fragment.getHeightData().getUnit(0, 2), 4240.0f));
                CAPTURE(fragment.getHeightData().getUnit(1, 2));
                CHECK(isCloseEnough(fragment.getHeightData().getUnit(1, 2), 4241.0f));
                CAPTURE(fragment.getHeightData().getUnit(2, 2));
                CHECK(isCloseEnough(fragment.getHeightData().getUnit(2, 2), 4242.0f));
                CAPTURE(fragment.getHeightData().getUnit(3, 2));
                CHECK(isCloseEnough(fragment.getHeightData().getUnit(3, 2), 4243.0f));
                CAPTURE(fragment.getHeightData().getUnit(0, 3));
                CHECK(isCloseEnough(fragment.getHeightData().getUnit(0, 3), 4340.0f));
                CAPTURE(fragment.getHeightData().getUnit(1, 3));
                CHECK(isCloseEnough(fragment.getHeightData().getUnit(1, 3), 4341.0f));
                CAPTURE(fragment.getHeightData().getUnit(2, 3));
                CHECK(isCloseEnough(fragment.getHeightData().getUnit(2, 3), 4342.0f));
                CAPTURE(fragment.getHeightData().getUnit(3, 3));
                CHECK(isCloseEnough(fragment.getHeightData().getUnit(3, 3), 4343.0f));
            }   
        }   
    }   
}
