#define CATCH_CONFIG_MAIN
#include "../catch.hpp"
#include "../../src/utilities/interpolationgrid3d.hpp"

const float accuracy = 0.0005f;
bool closeEnough(float a, float b)
{
    return fabs(a - b) < accuracy;
}

SCENARIO("Grid3Ds are fillable on creation","[utilities]")
{
    WHEN("An interpolation grid is created with a specific value")
    {   
        InterpolationGrid3D<32, 32, float> grid(1.0f);

        THEN("The grid contains said value")
        {
            REQUIRE(closeEnough(grid.at({0, 0, 0}), 1.0f));
        }   
    }   
}

SCENARIO("Values are settable and gettable in the grid","[utilities]")
{
    WHEN("An interpolation grid is created and some values are set")
    {   
        InterpolationGrid3D<32, 32, float> grid(0.0f);

        grid.set({0,  0 , 4}, 1.0f);
        grid.set({31, 31, 3}, 5.0f);
        grid.set({5,  10, 2}, 2.0f);

        THEN("The grid contains said values")
        {
            REQUIRE(closeEnough(grid.at({0,  0 , 4}), 1.0f));
            REQUIRE(closeEnough(grid.at({31, 31, 3}), 5.0f));
            REQUIRE(closeEnough(grid.at({5,  10, 2}), 2.0f));
            REQUIRE(closeEnough(grid.at({1,  1 , 1}), 0.0f));
        }   
    }   
}
