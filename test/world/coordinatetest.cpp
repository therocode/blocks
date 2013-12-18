#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include "world/worldstd.h"

TEST_CASE("worldToVoxel", "[coord]")
{
    REQUIRE(worldToVoxel(glm::vec3(0.0f, 0.0f, 0.0f)) == VoxelCoord(0, 0, 0));
    REQUIRE(worldToVoxel(glm::vec3(1.4f, 1.4f, 1.4f)) == VoxelCoord(1, 1, 1));
    REQUIRE(worldToVoxel(glm::vec3(-0.4f, -0.4f, -0.4f)) == VoxelCoord(-1, -1, -1));
}

TEST_CASE("voxelToWorld", "[coord]")
{
    REQUIRE(voxelToWorld(VoxelCoord(0, 0, 0)) == glm::vec3(0.0f, 0.0f, 0.0f));
    REQUIRE(voxelToWorld(VoxelCoord(1, 1, 1)) == glm::vec3(1.0f, 1.0f, 1.0f));
    REQUIRE(voxelToWorld(VoxelCoord(-3, -3, -3)) == glm::vec3(-3.0f, -3.0f, -3.0f));
}

TEST_CASE("chunkToWorld", "[coord]")
{
    REQUIRE(chunkToWorld(ChunkCoord(0, 0, 0)) == glm::vec3(0.0f, 0.0f, 0.0f));
    REQUIRE(chunkToWorld(ChunkCoord(1, 1, 1)) == glm::vec3(1.0f * chunkWidth, 1.0f * chunkWidth, 1.0f * chunkWidth));
    REQUIRE(chunkToWorld(ChunkCoord(-2, -2, -2)) == glm::vec3(-2.0f * chunkWidth, -2.0f * chunkWidth, -2.0f * chunkWidth));
}

TEST_CASE("regionToWorld", "[coord]")
{
    REQUIRE(regionToWorld(RegionCoord(0, 0, 0)) == glm::vec3(0.0f, 0.0f, 0.0f));
    REQUIRE(regionToWorld(RegionCoord(1, 1, 1)) == glm::vec3(1.0f * regionWidth, 1.0f * regionWidth, 1.0f * regionWidth));
    REQUIRE(regionToWorld(RegionCoord(-2, -2, -2)) == glm::vec3(-2.0f * regionWidth, -2.0f * regionWidth, -2.0f * regionWidth));
}
