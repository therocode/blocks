#define CATCH_CONFIG_MAIN
#include "../catch.hpp"
#include "../../src/utilities/interpolationgrid3d.hpp"

const float accuracy = 0.0005f;
bool closeEnough(float a, float b)
{
    return fabs(a - b) < accuracy;
}

//non-interpolating - i.e. downsampling = 0

SCENARIO("Grid3Ds are fillable on creation","[utilities]")
{
    WHEN("An interpolation grid is created with a specific value")
    {   
        InterpolationGrid3D<float> grid(32, 0, 1.0f);

        THEN("The grid contains said value")
        {
            REQUIRE(closeEnough(grid.get({0, 0, 0}), 1.0f));
        }   
    }   
}

SCENARIO("Values are settable and gettable in the inner grid","[utilities]")
{
    WHEN("An interpolation grid is created and some values are set")
    {   
        InterpolationGrid3D<float> grid(32, 0, 0.0f);

        grid.setInner({0,  0 , 4}, 1.0f);
        grid.setInner({31, 31, 3}, 5.0f);
        grid.setInner({5,  10, 2}, 2.0f);

        THEN("The grid contains said values")
        {
            REQUIRE(closeEnough(grid.get({0,  0 , 4}), 1.0f));
            REQUIRE(closeEnough(grid.get({31, 31, 3}), 5.0f));
            REQUIRE(closeEnough(grid.get({5,  10, 2}), 2.0f));
            REQUIRE(closeEnough(grid.get({1,  1 , 1}), 0.0f));
        }   
    }   
}

SCENARIO("Values are equal to the inner grid","[utilities]")
{
    WHEN("An interpolation grid is created and some values are set")
    {   
        InterpolationGrid3D<float> grid(32, 0, 0.0f);

        grid.setInner({0,  0 , 4}, 1.0f);
        grid.setInner({31, 31, 3}, 5.0f);
        grid.setInner({5,  10, 2}, 2.0f);

        THEN("The grid contains said values in the inner grid")
        {
            REQUIRE(closeEnough(grid.getInner({0,  0 , 4}), 1.0f));
            REQUIRE(closeEnough(grid.getInner({31, 31, 3}), 5.0f));
            REQUIRE(closeEnough(grid.getInner({5,  10, 2}), 2.0f));
            REQUIRE(closeEnough(grid.getInner({1,  1 , 1}), 0.0f));
        }   
    }   
}

//interpolating
