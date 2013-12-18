#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include "world/worldstd.h"

TEST_CASE("wrap", "[wrap]")
{
    REQUIRE(wrapInt(-5, 0, 3) == 3);
    REQUIRE(wrapInt(-4, 0, 3) == 0);
    REQUIRE(wrapInt(-3, 0, 3) == 1);
    REQUIRE(wrapInt(-2, 0, 3) == 2);
    REQUIRE(wrapInt(-1, 0, 3) == 3);
    REQUIRE(wrapInt(0, 0, 3) == 0);
    REQUIRE(wrapInt(1, 0, 3) == 1);
    REQUIRE(wrapInt(2, 0, 3) == 2);
    REQUIRE(wrapInt(3, 0, 3) == 3);
    REQUIRE(wrapInt(4, 0, 3) == 0);
    REQUIRE(wrapInt(5, 0, 3) == 1);
}

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

TEST_CASE("voxelToChunk", "[coord]")
{
    REQUIRE(voxelToChunk(VoxelCoord(0, 0, 0)) == ChunkCoord(0, 0, 0));
    REQUIRE(voxelToChunk(VoxelCoord(17, 17, 17)) == ChunkCoord(1, 1, 1));
    REQUIRE(voxelToChunk(VoxelCoord(-14, -14, -14)) == ChunkCoord(-1, -1, -1));
    REQUIRE(voxelToChunk(VoxelCoord(-15, -15, -15)) == ChunkCoord(-1, -1, -1));
    REQUIRE(voxelToChunk(VoxelCoord(-16, -16, -16)) == ChunkCoord(-1, -1, -1));
    REQUIRE(voxelToChunk(VoxelCoord(-17, -17, -17)) == ChunkCoord(-2, -2, -2));
}

TEST_CASE("voxelToRegion", "[coord]")
{
    REQUIRE(voxelToRegion(VoxelCoord(0, 0, 0)) == RegionCoord(0, 0, 0));
    REQUIRE(voxelToRegion(VoxelCoord(899, 899, 899)) == RegionCoord(1, 1, 1));
    REQUIRE(voxelToRegion(VoxelCoord(-200, -200, -200)) == RegionCoord(-1, -1, -1));
    REQUIRE(voxelToRegion(VoxelCoord(-600, -600, -600)) == RegionCoord(-2, -2, -2));
}

TEST_CASE("chunkToRegion", "[coord]")
{
    REQUIRE(chunkToRegion(ChunkCoord(0, 0, 0)) == RegionCoord(0, 0, 0));
    REQUIRE(chunkToRegion(ChunkCoord(46, 46, 46)) == RegionCoord(1, 1, 1));
    REQUIRE(chunkToRegion(ChunkCoord(-25, -25, -25)) == RegionCoord(-1, -1, -1));
    REQUIRE(chunkToRegion(ChunkCoord(-35, -35, -35)) == RegionCoord(-2, -2, -2));
}

TEST_CASE("voxelToChunkVoxel", "[coord]")
{
    REQUIRE(voxelToChunkVoxel(VoxelCoord(0, 0, 0)) == ChunkVoxelCoord(0, 0, 0));
    REQUIRE(voxelToChunkVoxel(VoxelCoord(15, 15, 15)) == ChunkVoxelCoord(15, 15, 15));
    REQUIRE(voxelToChunkVoxel(VoxelCoord(16, 16, 16)) == ChunkVoxelCoord(0, 0, 0));
    REQUIRE(voxelToChunkVoxel(VoxelCoord(17, 17, 17)) == ChunkVoxelCoord(1, 1, 1));
    REQUIRE(voxelToChunkVoxel(VoxelCoord(-6, -6, -6)) == ChunkVoxelCoord(10, 10, 10));
}

TEST_CASE("voxelToRegionChunk", "[coord]")
{
    REQUIRE(voxelToRegionChunk(VoxelCoord(0, 0, 0)) == RegionChunkCoord(0, 0, 0));
    REQUIRE(voxelToRegionChunk(VoxelCoord(15, 15, 15)) == RegionChunkCoord(0, 0, 0));
    REQUIRE(voxelToRegionChunk(VoxelCoord(16, 16, 16)) == RegionChunkCoord(1, 1, 1));
    REQUIRE(voxelToRegionChunk(VoxelCoord(515, 515, 515)) == RegionChunkCoord(0, 0, 0));
    REQUIRE(voxelToRegionChunk(VoxelCoord(-5, -5, -5)) == RegionChunkCoord(31, 31, 31));
    REQUIRE(voxelToRegionChunk(VoxelCoord(-16, -16, -16)).x == RegionChunkCoord(31, 31, 31).x);
    REQUIRE(voxelToRegionChunk(VoxelCoord(-17, -17, -17)).x == RegionChunkCoord(30, 30, 30).x);
    REQUIRE(voxelToRegionChunk(VoxelCoord(512, 512, 512)) == RegionChunkCoord(0, 0, 0));
    REQUIRE(voxelToRegionChunk(VoxelCoord(-512, -512, -512)).x == RegionChunkCoord(0, 0, 0).x);
}
