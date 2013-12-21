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
    REQUIRE(worldToVoxel(glm::vec3(1.9f, 1.9f, 1.9f)) == VoxelCoord(1, 1, 1));
    REQUIRE(worldToVoxel(glm::vec3(2.5f, 2.5f, 2.5f)) == VoxelCoord(2, 2, 2));
    REQUIRE(worldToVoxel(glm::vec3(-0.9f, -0.9f, -0.9f)) == VoxelCoord(-1, -1, -1));
    REQUIRE(worldToVoxel(glm::vec3(-1.9f, -1.9f, -1.9f)) == VoxelCoord(-2, -2, -2));
}

TEST_CASE("worldToChunk", "[coord]")
{
    REQUIRE(worldToChunk(glm::vec3(0.0f, 0.0f, 0.0f)) == ChunkCoord(0, 0, 0));
    REQUIRE(worldToChunk(glm::vec3(1.0f, 1.0f, 1.0f)) == ChunkCoord(0, 0, 0));
    REQUIRE(worldToChunk(glm::vec3(-1.0f, -1.0f, -1.0f)) == ChunkCoord(-1, -1, -1));
    REQUIRE(worldToChunk(glm::vec3(15.9f, 15.9f, 15.9f)) == ChunkCoord(0, 0, 0));
    REQUIRE(worldToChunk(glm::vec3(16.5f, 16.5f, 16.5f)) == ChunkCoord(1, 1, 1));
    REQUIRE(worldToChunk(glm::vec3(-16.0f, -16.0f, -16.0f)) == ChunkCoord(-1, -1, -1));
    REQUIRE(worldToChunk(glm::vec3(-16.9f, -16.9f, -16.9f)) == ChunkCoord(-2, -2, -2));
}

TEST_CASE("worldToRegion", "[coord]")
{
    REQUIRE(worldToRegion(glm::vec3(0.0f, 0.0f, 0.0f)) == RegionCoord(0, 0, 0));
    REQUIRE(worldToRegion(glm::vec3(1.0f, 1.0f, 1.0f)) == RegionCoord(0, 0, 0));
    REQUIRE(worldToRegion(glm::vec3(-1.0f, -1.0f, -1.0f)) == RegionCoord(-1, -1, -1));
    REQUIRE(worldToRegion(glm::vec3(511.9f, 511.9f, 511.9f)) == RegionCoord(0, 0, 0));
    REQUIRE(worldToRegion(glm::vec3(512.5f, 512.5f, 512.5f)) == RegionCoord(1, 1, 1));
    REQUIRE(worldToRegion(glm::vec3(-512.0f, -512.0f, -512.0f)) == RegionCoord(-1, -1, -1));
    REQUIRE(worldToRegion(glm::vec3(-512.9f, -512.9f, -512.9f)) == RegionCoord(-2, -2, -2));
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
    REQUIRE(regionToWorld(RegionCoord(1, 1, 1)) == glm::vec3(1.0f * regionWidth * chunkWidth, 1.0f * regionWidth * chunkWidth, 1.0f * regionWidth * chunkWidth));
    REQUIRE(regionToWorld(RegionCoord(-2, -2, -2)) == glm::vec3(-2.0f * regionWidth * chunkWidth, -2.0f * regionWidth * chunkWidth, -2.0f * regionWidth * chunkWidth));
}

TEST_CASE("voxelToChunk", "[coord]")
{
    REQUIRE(voxelToChunk(VoxelCoord(0, 0, 0)) == ChunkCoord(0, 0, 0));
    REQUIRE(voxelToChunk(VoxelCoord(1, 1, 1)) == ChunkCoord(0, 0, 0));
    REQUIRE(voxelToChunk(VoxelCoord(-1, -1, -1)) == ChunkCoord(-1, -1, -1));
    REQUIRE(voxelToChunk(VoxelCoord(14, 14, 14)) == ChunkCoord(0, 0, 0));
    REQUIRE(voxelToChunk(VoxelCoord(15, 15, 15)) == ChunkCoord(0, 0, 0));
    REQUIRE(voxelToChunk(VoxelCoord(16, 16, 16)) == ChunkCoord(1, 1, 1));
    REQUIRE(voxelToChunk(VoxelCoord(17, 17, 17)) == ChunkCoord(1, 1, 1));
    REQUIRE(voxelToChunk(VoxelCoord(-14, -14, -14)) == ChunkCoord(-1, -1, -1));
    REQUIRE(voxelToChunk(VoxelCoord(-15, -15, -15)) == ChunkCoord(-1, -1, -1));
    REQUIRE(voxelToChunk(VoxelCoord(-16, -16, -16)) == ChunkCoord(-1, -1, -1));
    REQUIRE(voxelToChunk(VoxelCoord(-17, -17, -17)) == ChunkCoord(-2, -2, -2));
}

TEST_CASE("voxelToRegion", "[coord]")
{
    REQUIRE(voxelToRegion(VoxelCoord(0, 0, 0)) == RegionCoord(0, 0, 0));
    REQUIRE(voxelToRegion(VoxelCoord(1, 1, 1)) == RegionCoord(0, 0, 0));
    REQUIRE(voxelToRegion(VoxelCoord(-1, -1, -1)) == RegionCoord(-1, -1, -1));
    REQUIRE(voxelToRegion(VoxelCoord(511, 511, 511)) == RegionCoord(0, 0, 0));
    REQUIRE(voxelToRegion(VoxelCoord(512, 512, 512)) == RegionCoord(1, 1, 1));
    REQUIRE(voxelToRegion(VoxelCoord(513, 513, 513)) == RegionCoord(1, 1, 1));
    REQUIRE(voxelToRegion(VoxelCoord(-511, -511, -511)) == RegionCoord(-1, -1, -1));
    REQUIRE(voxelToRegion(VoxelCoord(-512, -512, -512)) == RegionCoord(-1, -1, -1));
    REQUIRE(voxelToRegion(VoxelCoord(-513, -513, -513)) == RegionCoord(-2, -2, -2));
}

TEST_CASE("chunkToRegion", "[coord]")
{
    REQUIRE(chunkToRegion(ChunkCoord(0, 0, 0)) == RegionCoord(0, 0, 0));
    REQUIRE(chunkToRegion(ChunkCoord(1, 1, 1)) == RegionCoord(0, 0, 0));
    REQUIRE(chunkToRegion(ChunkCoord(-1, -1, -1)) == RegionCoord(-1, -1, -1));
    REQUIRE(chunkToRegion(ChunkCoord(31, 31, 31)) == RegionCoord(0, 0, 0));
    REQUIRE(chunkToRegion(ChunkCoord(32, 32, 32)) == RegionCoord(1, 1, 1));
    REQUIRE(chunkToRegion(ChunkCoord(33, 33, 33)) == RegionCoord(1, 1, 1));
    REQUIRE(chunkToRegion(ChunkCoord(-31, -31, -31)) == RegionCoord(-1, -1, -1));
    REQUIRE(chunkToRegion(ChunkCoord(-32, -32, -32)) == RegionCoord(-1, -1, -1));
    REQUIRE(chunkToRegion(ChunkCoord(-33, -33, -33)) == RegionCoord(-2, -2, -2));
}

TEST_CASE("voxelToChunkVoxel", "[coord]")
{
    REQUIRE(voxelToChunkVoxel(VoxelCoord(0, 0, 0)) == ChunkVoxelCoord(0, 0, 0));
    REQUIRE(voxelToChunkVoxel(VoxelCoord(1, 1, 1)) == ChunkVoxelCoord(1, 1, 1));
    REQUIRE(voxelToChunkVoxel(VoxelCoord(-1, -1, -1)) == ChunkVoxelCoord(15, 15, 15));
    REQUIRE(voxelToChunkVoxel(VoxelCoord(-13, -13, -13)) == ChunkVoxelCoord(3, 3, 3));
    REQUIRE(voxelToChunkVoxel(VoxelCoord(-14, -14, -14)) == ChunkVoxelCoord(2, 2, 2));
    REQUIRE(voxelToChunkVoxel(VoxelCoord(-15, -15, -15)) == ChunkVoxelCoord(1, 1, 1));
    REQUIRE(voxelToChunkVoxel(VoxelCoord(-16, -16, -16)) == ChunkVoxelCoord(0, 0, 0));
    REQUIRE(voxelToChunkVoxel(VoxelCoord(-17, -17, -17)) == ChunkVoxelCoord(15, 15, 15));
    REQUIRE(voxelToChunkVoxel(VoxelCoord(13, 13, 13)) == ChunkVoxelCoord(13, 13, 13));
    REQUIRE(voxelToChunkVoxel(VoxelCoord(14, 14, 14)) == ChunkVoxelCoord(14, 14, 14));
    REQUIRE(voxelToChunkVoxel(VoxelCoord(15, 15, 15)) == ChunkVoxelCoord(15, 15, 15));
    REQUIRE(voxelToChunkVoxel(VoxelCoord(16, 16, 16)) == ChunkVoxelCoord(0, 0, 0));
    REQUIRE(voxelToChunkVoxel(VoxelCoord(17, 17, 17)) == ChunkVoxelCoord(1, 1, 1));
}

TEST_CASE("voxelToRegionChunk", "[coord]")
{
    REQUIRE(voxelToRegionChunk(VoxelCoord(0, 0, 0)) == RegionChunkCoord(0, 0, 0));
    REQUIRE(voxelToRegionChunk(VoxelCoord(1, 1, 1)) == RegionChunkCoord(0, 0, 0));
    REQUIRE(voxelToRegionChunk(VoxelCoord(-1, -1, -1)) == RegionChunkCoord(31, 31, 31));

    REQUIRE(voxelToRegionChunk(VoxelCoord(14, 14, 14)) == RegionChunkCoord(0, 0, 0));
    REQUIRE(voxelToRegionChunk(VoxelCoord(15, 15, 15)) == RegionChunkCoord(0, 0, 0));
    REQUIRE(voxelToRegionChunk(VoxelCoord(16, 16, 16)) == RegionChunkCoord(1, 1, 1));
    REQUIRE(voxelToRegionChunk(VoxelCoord(17, 17, 17)) == RegionChunkCoord(1, 1, 1));

    REQUIRE(voxelToRegionChunk(VoxelCoord(-14, -14, -14)) == RegionChunkCoord(31, 31, 31));
    REQUIRE(voxelToRegionChunk(VoxelCoord(-15, -15, -15)) == RegionChunkCoord(31, 31, 31));
    REQUIRE(voxelToRegionChunk(VoxelCoord(-16, -16, -16)) == RegionChunkCoord(31, 31, 31));
    REQUIRE(voxelToRegionChunk(VoxelCoord(-17, -17, -17)) == RegionChunkCoord(30, 30, 30));

    REQUIRE(voxelToRegionChunk(VoxelCoord(510, 510, 510)) == RegionChunkCoord(31, 31, 31));
    REQUIRE(voxelToRegionChunk(VoxelCoord(511, 511, 511)) == RegionChunkCoord(31, 31, 31));
    REQUIRE(voxelToRegionChunk(VoxelCoord(512, 512, 512)) == RegionChunkCoord(0, 0, 0));
    REQUIRE(voxelToRegionChunk(VoxelCoord(513, 513, 513)) == RegionChunkCoord(0, 0, 0));

    REQUIRE(voxelToRegionChunk(VoxelCoord(-510, -510, -510)) == RegionChunkCoord(0, 0, 0));
    REQUIRE(voxelToRegionChunk(VoxelCoord(-511, -511, -511)) == RegionChunkCoord(0, 0, 0));
    REQUIRE(voxelToRegionChunk(VoxelCoord(-512, -512, -512)) == RegionChunkCoord(0, 0, 0));
    REQUIRE(voxelToRegionChunk(VoxelCoord(-513, -513, -513)) == RegionChunkCoord(31, 31, 31));
}

TEST_CASE("chunkToRegionChunk", "[coord]")
{
    REQUIRE(chunkToRegionChunk(ChunkCoord(0, 0, 0)) == RegionChunkCoord(0, 0, 0));
    REQUIRE(chunkToRegionChunk(ChunkCoord(1, 1, 1)) == RegionChunkCoord(1, 1, 1));
    REQUIRE(chunkToRegionChunk(ChunkCoord(-1, -1, -1)) == RegionChunkCoord(31, 31, 31));

    REQUIRE(chunkToRegionChunk(ChunkCoord(30, 30, 30)) == RegionChunkCoord(30, 30, 30));
    REQUIRE(chunkToRegionChunk(ChunkCoord(31, 31, 31)) == RegionChunkCoord(31, 31, 31));
    REQUIRE(chunkToRegionChunk(ChunkCoord(32, 32, 32)) == RegionChunkCoord(0, 0, 0));
    REQUIRE(chunkToRegionChunk(ChunkCoord(33, 33, 33)) == RegionChunkCoord(1, 1, 1));

    REQUIRE(chunkToRegionChunk(ChunkCoord(-30, -30, -30)) == RegionChunkCoord(2, 2, 2));
    REQUIRE(chunkToRegionChunk(ChunkCoord(-31, -31, -31)) == RegionChunkCoord(1, 1, 1));
    REQUIRE(chunkToRegionChunk(ChunkCoord(-32, -32, -32)) == RegionChunkCoord(0, 0, 0));
    REQUIRE(chunkToRegionChunk(ChunkCoord(-33, -33, -33)) == RegionChunkCoord(31, 31, 31));
}

TEST_CASE("voxelToRegionVoxel", "[coord]")
{
    REQUIRE(voxelToRegionVoxel(VoxelCoord(0, 0, 0)) == RegionVoxelCoord(0, 0));
    REQUIRE(voxelToRegionVoxel(VoxelCoord(1, 1, 1)) == RegionVoxelCoord(1, 1));
    REQUIRE(voxelToRegionVoxel(VoxelCoord(-1, -1, -1)) == RegionVoxelCoord(511, 511));

    REQUIRE(voxelToRegionVoxel(VoxelCoord(510, 510, 510)) == RegionVoxelCoord(510, 510));
    REQUIRE(voxelToRegionVoxel(VoxelCoord(511, 511, 511)) == RegionVoxelCoord(511, 511));
    REQUIRE(voxelToRegionVoxel(VoxelCoord(512, 512, 512)) == RegionVoxelCoord(0, 0));
    REQUIRE(voxelToRegionVoxel(VoxelCoord(513, 513, 513)) == RegionVoxelCoord(1, 1));

    REQUIRE(voxelToRegionVoxel(VoxelCoord(-510, -510, -510)) == RegionVoxelCoord(2, 2));
    REQUIRE(voxelToRegionVoxel(VoxelCoord(-511, -511, -511)) == RegionVoxelCoord(1, 1));
    REQUIRE(voxelToRegionVoxel(VoxelCoord(-512, -512, -512)) == RegionVoxelCoord(0, 0));
    REQUIRE(voxelToRegionVoxel(VoxelCoord(-513, -513, -513)) == RegionVoxelCoord(511, 511));
}
