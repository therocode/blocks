#define CATCH_CONFIG_MAIN
#include "../catch.hpp"
#include "../../src/utilities/interpolationgrid3d.hpp"
#include "../../src/utilities/interpolators.hpp"

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

SCENARIO("Grids can be filled with values from a vector", "[utilities]")
{
    WHEN("A grid is filled with data from a vector")
    {
        std::vector<float> data({0.0f, 0.0f, 
                                 1.0f, 0.0f,

                                 0.0f, 0.0f,
                                 0.0f, 2.0f});

        InterpolationGrid3D<float> grid(2, 0, data);

        THEN("The grid contains the data given")
        {
            REQUIRE(closeEnough(grid.get({0, 0, 0}), 0.0f));
            REQUIRE(closeEnough(grid.get({0, 1, 0}), 1.0f));
            REQUIRE(closeEnough(grid.get({1, 1, 1}), 2.0f));
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

SCENARIO("Values can be interpolated","[utilities]")
{
    WHEN("SMALLEST SIZE: An interpolation grid is created where the inner grid is smaller than the size and given an interpolator")
    {   
        InterpolationGrid3D<float> grid(32, 5, 0.0f);

        grid.setInner({0,  0 , 0}, 1.0f);
        grid.setInner({1,  1 , 1}, 2.0f);

        grid.setInterpolator(Interpolator<float>::nearestNeigbor);

        THEN("The grid returns interpolated values")
        {
            REQUIRE(closeEnough(grid.get({0,  0 , 0}),  1.0f));
            REQUIRE(closeEnough(grid.get({3,  7,  12}), 1.0f));
            REQUIRE(closeEnough(grid.get({3,  7,  18}), 0.0f));
            REQUIRE(closeEnough(grid.get({20,  19 , 31}), 2.0f));
        }   
    }   
    
    WHEN("SMALLEST SIZE: An interpolation grid is created where the inner grid is smaller than the size and given an interpolator")
    {   
        InterpolationGrid3D<float> grid(32, 4, 0.0f);

        grid.setInner({0,  0 , 0}, 1.0f);
        grid.setInner({2,  2 , 2}, 2.0f);
        grid.setInner({2,  0 , 1}, 4.0f);
        grid.setInner({1,  1 , 1}, 5.0f);

        grid.setInterpolator(Interpolator<float>::nearestNeigbor);

        THEN("The grid returns interpolated values")
        {
            REQUIRE(closeEnough(grid.get({0,  0 , 0}),  1.0f));
            REQUIRE(closeEnough(grid.get({2,  4 , 7}),  1.0f));

            REQUIRE(closeEnough(grid.get({31,  30,  24}), 2.0f));
            REQUIRE(closeEnough(grid.get({29,  28,  27}), 2.0f));

            REQUIRE(closeEnough(grid.get({25,  5, 8}), 4.0f));
            REQUIRE(closeEnough(grid.get({27,  3, 23}), 4.0f));

            REQUIRE(closeEnough(grid.get({9,  13,  18}), 5.0f));
            REQUIRE(closeEnough(grid.get({16,  20, 21}), 5.0f));
        }   
    }   
}
