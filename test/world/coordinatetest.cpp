#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include "world/worlddefines.hpp"

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

TEST_CASE("ChunkToVoxel", "[coord]")
{
    REQUIRE(ChunkToVoxel::convert(ChunkCoord(0, 0, 0)) ==       VoxelCoord(0, 0, 0));
    REQUIRE(ChunkToVoxel::convert(ChunkCoord(1, 1, 1)) ==       VoxelCoord(16, 16, 16));
    REQUIRE(ChunkToVoxel::convert(ChunkCoord(-1, -1, -1)) ==    VoxelCoord(-16, -16, -16));

    REQUIRE(ChunkToVoxel::convert(ChunkCoord(30, 30, 30)) ==    VoxelCoord(480, 480, 480));

    REQUIRE(ChunkToVoxel::convert(ChunkCoord(-30, -30, -30)) == VoxelCoord(-480, -480, -480));
}

TEST_CASE("WorldToVoxel", "[coord]")
{
    REQUIRE(WorldToVoxel::convert(glm::vec3(0.0f, 0.0f, 0.0f)) == VoxelCoord(0, 0, 0));
    REQUIRE(WorldToVoxel::convert(glm::vec3(1.9f, 1.9f, 1.9f)) == VoxelCoord(1, 1, 1));
    REQUIRE(WorldToVoxel::convert(glm::vec3(2.5f, 2.5f, 2.5f)) == VoxelCoord(2, 2, 2));
    REQUIRE(WorldToVoxel::convert(glm::vec3(-0.9f, -0.9f, -0.9f)) == VoxelCoord(-1, -1, -1));
    REQUIRE(WorldToVoxel::convert(glm::vec3(-1.9f, -1.9f, -1.9f)) == VoxelCoord(-2, -2, -2));
}

TEST_CASE("WorldToChunk", "[coord]")
{
    REQUIRE(WorldToChunk::convert(glm::vec3(0.0f, 0.0f, 0.0f)) == ChunkCoord(0, 0, 0));
    REQUIRE(WorldToChunk::convert(glm::vec3(1.0f, 1.0f, 1.0f)) == ChunkCoord(0, 0, 0));
    REQUIRE(WorldToChunk::convert(glm::vec3(-1.0f, -1.0f, -1.0f)) == ChunkCoord(-1, -1, -1));
    REQUIRE(WorldToChunk::convert(glm::vec3(15.9f, 15.9f, 15.9f)) == ChunkCoord(0, 0, 0));
    REQUIRE(WorldToChunk::convert(glm::vec3(16.5f, 16.5f, 16.5f)) == ChunkCoord(1, 1, 1));
    REQUIRE(WorldToChunk::convert(glm::vec3(-16.0f, -16.0f, -16.0f)) == ChunkCoord(-1, -1, -1));
    REQUIRE(WorldToChunk::convert(glm::vec3(-16.9f, -16.9f, -16.9f)) == ChunkCoord(-2, -2, -2));
}

TEST_CASE("VoxelToWorld", "[coord]")
{
    REQUIRE(VoxelToWorld::convert(VoxelCoord(0, 0, 0)) == glm::vec3(0.0f, 0.0f, 0.0f));
    REQUIRE(VoxelToWorld::convert(VoxelCoord(1, 1, 1)) == glm::vec3(1.0f, 1.0f, 1.0f));
    REQUIRE(VoxelToWorld::convert(VoxelCoord(-3, -3, -3)) == glm::vec3(-3.0f, -3.0f, -3.0f));
}

TEST_CASE("ChunkToWorld", "[coord]")
{
    REQUIRE(ChunkToWorld::convert(ChunkCoord(0, 0, 0)) == glm::vec3(0.0f, 0.0f, 0.0f));
    REQUIRE(ChunkToWorld::convert(ChunkCoord(1, 1, 1)) == glm::vec3(1.0f * chunkWidth, 1.0f * chunkWidth, 1.0f * chunkWidth));
    REQUIRE(ChunkToWorld::convert(ChunkCoord(-2, -2, -2)) == glm::vec3(-2.0f * chunkWidth, -2.0f * chunkWidth, -2.0f * chunkWidth));
}

TEST_CASE("VoxelToChunk", "[coord]")
{
    REQUIRE(VoxelToChunk::convert(VoxelCoord(0, 0, 0)) == ChunkCoord(0, 0, 0));
    REQUIRE(VoxelToChunk::convert(VoxelCoord(1, 1, 1)) == ChunkCoord(0, 0, 0));
    REQUIRE(VoxelToChunk::convert(VoxelCoord(-1, -1, -1)) == ChunkCoord(-1, -1, -1));
    REQUIRE(VoxelToChunk::convert(VoxelCoord(14, 14, 14)) == ChunkCoord(0, 0, 0));
    REQUIRE(VoxelToChunk::convert(VoxelCoord(15, 15, 15)) == ChunkCoord(0, 0, 0));
    REQUIRE(VoxelToChunk::convert(VoxelCoord(16, 16, 16)) == ChunkCoord(1, 1, 1));
    REQUIRE(VoxelToChunk::convert(VoxelCoord(17, 17, 17)) == ChunkCoord(1, 1, 1));
    REQUIRE(VoxelToChunk::convert(VoxelCoord(-14, -14, -14)) == ChunkCoord(-1, -1, -1));
    REQUIRE(VoxelToChunk::convert(VoxelCoord(-15, -15, -15)) == ChunkCoord(-1, -1, -1));
    REQUIRE(VoxelToChunk::convert(VoxelCoord(-16, -16, -16)) == ChunkCoord(-1, -1, -1));
    REQUIRE(VoxelToChunk::convert(VoxelCoord(-17, -17, -17)) == ChunkCoord(-2, -2, -2));
}

TEST_CASE("VoxelToChunkVoxel", "[coord]")
{
    REQUIRE(VoxelToChunkVoxel::convert(VoxelCoord(0, 0, 0)) == ChunkVoxelCoord(0, 0, 0));
    REQUIRE(VoxelToChunkVoxel::convert(VoxelCoord(1, 1, 1)) == ChunkVoxelCoord(1, 1, 1));
    REQUIRE(VoxelToChunkVoxel::convert(VoxelCoord(-1, -1, -1)) == ChunkVoxelCoord(15, 15, 15));
    REQUIRE(VoxelToChunkVoxel::convert(VoxelCoord(-13, -13, -13)) == ChunkVoxelCoord(3, 3, 3));
    REQUIRE(VoxelToChunkVoxel::convert(VoxelCoord(-14, -14, -14)) == ChunkVoxelCoord(2, 2, 2));
    REQUIRE(VoxelToChunkVoxel::convert(VoxelCoord(-15, -15, -15)) == ChunkVoxelCoord(1, 1, 1));
    REQUIRE(VoxelToChunkVoxel::convert(VoxelCoord(-16, -16, -16)) == ChunkVoxelCoord(0, 0, 0));
    REQUIRE(VoxelToChunkVoxel::convert(VoxelCoord(-17, -17, -17)) == ChunkVoxelCoord(15, 15, 15));
    REQUIRE(VoxelToChunkVoxel::convert(VoxelCoord(13, 13, 13)) == ChunkVoxelCoord(13, 13, 13));
    REQUIRE(VoxelToChunkVoxel::convert(VoxelCoord(14, 14, 14)) == ChunkVoxelCoord(14, 14, 14));
    REQUIRE(VoxelToChunkVoxel::convert(VoxelCoord(15, 15, 15)) == ChunkVoxelCoord(15, 15, 15));
    REQUIRE(VoxelToChunkVoxel::convert(VoxelCoord(16, 16, 16)) == ChunkVoxelCoord(0, 0, 0));
    REQUIRE(VoxelToChunkVoxel::convert(VoxelCoord(17, 17, 17)) == ChunkVoxelCoord(1, 1, 1));
}
