#include "../catch.hpp"
#include "../testhelpers.hpp"

#include "../../src/utilities/interpolationgrid3d.hpp"
#include "../../src/utilities/interpolators.hpp"

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
    WHEN("An interpolation grid is created")
    {   
        InterpolationGrid3D<float> grid(256, 2, 0.0f);

        grid.setInner({0,  0 , 0}, 1.0f);
        grid.setInner({1,  1 , 1}, 2.0f);
        grid.setInner({0,  0 , 2}, 3.0f);

        grid.setInterpolator(Interpolator<float>::nearestNeigbor);

        THEN("The grid returns interpolated values")
        {
            CHECK(closeEnough(grid.get({0, 0, 0}), 1.0f));
            CHECK(closeEnough(grid.get({1, 1, 1}), 1.0f));
            CHECK(closeEnough(grid.get({2, 2, 2}), 2.0f));
            CHECK(closeEnough(grid.get({3, 3, 3}), 2.0f));
            CHECK(closeEnough(grid.get({4, 4, 4}), 2.0f));
            CHECK(closeEnough(grid.get({5, 5, 5}), 2.0f));
            CHECK(closeEnough(grid.get({6, 6, 6}), 0.0f));
            CHECK(closeEnough(grid.get({7, 7, 7}), 0.0f));
            CHECK(closeEnough(grid.get({0, 0, 8}), 3.0f));
            CHECK(closeEnough(grid.get({0, 0, 9}), 3.0f));
            CHECK(closeEnough(grid.get({0, 0, 10}), 0.0f));
            CHECK(closeEnough(grid.get({0, 0, 11}), 0.0f));
            CHECK(closeEnough(grid.get({3, 3, 8}), 0.0f));
            CHECK(closeEnough(grid.get({3, 3, 9}), 0.0f));
            CHECK(closeEnough(grid.get({3, 3, 10}), 0.0f));
            CHECK(closeEnough(grid.get({3, 3, 11}), 0.0f));
            CHECK(closeEnough(grid.get({2, 2 ,5}), 2.0f));
            CHECK(closeEnough(grid.get({5, 2 ,5}), 2.0f));
        }   
    }   
}
