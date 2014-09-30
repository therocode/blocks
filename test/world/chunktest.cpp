#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include "world/chunk.hpp"

using ChunkVoxelCoord  = glm::u8vec3;

SCENARIO("Voxels can be set and gotten","[world]")
{
    GIVEN("A newly created chunk")
    {   
        Chunk chunk;

        WHEN("Any voxel is gotten")
        {   
            VoxelType type = chunk.getVoxelType({3, 3, 3});
            THEN("The return value is air")
            {   
                CHECK(type == 0);
            }
        }

        WHEN("A voxel is set to a particular value")
        {
            chunk.setVoxelType({5, 3, 1}, 20);
            THEN("The same value is gotten back when accessed, whilst other voxels are still air")
            {
                CHECK(chunk.getVoxelType({5, 3, 1}) == 20);
                CHECK(chunk.getVoxelType({1, 2, 3}) == 0);
            }
        }
    }
}

SCENARIO("Voxels can be set from data arrays","[world]")
{
    GIVEN("A chunk with some voxels set")
    {
        Chunk originalChunk;

        originalChunk.setVoxelType({10, 1, 2}, 1);
        originalChunk.setVoxelType({4, 2, 10}, 2);
        originalChunk.setVoxelType({5, 9, 4},  3);

        WHEN("A new chunk is created from the data of the original one")
        {
            Chunk newChunk(originalChunk.getLocation(), originalChunk.getFlatVoxelTypeData());

            THEN("The new chunk holds the same voxel data")
            {
                CHECK(newChunk.getVoxelType({10, 1, 2}) == 1);
                CHECK(newChunk.getVoxelType({4, 2, 10}) == 2);
                CHECK(newChunk.getVoxelType({5, 9, 4})  == 3);
                CHECK(newChunk.getVoxelType({15, 15, 15})  == 0);
            }
        }
    }
}

SCENARIO("Chunks can be decoded into flat voxel arrays","[world]")
{
    GIVEN("A chunk with some voxels set")
    {
        Chunk chunk;

        chunk.setVoxelType({10, 1, 2}, 1);
        chunk.setVoxelType({4, 2, 10}, 2);
        chunk.setVoxelType({5, 9, 4},  3);

        WHEN("A flat array is gotten")
        {
            VoxelTypeArray voxels = chunk.getFlatVoxelTypeData();

            THEN("The correct voxels are set in the array")
            {
                CHECK(voxels[0] == 0);
                CHECK(voxels[4095] == 0);
                CHECK(voxels[10 + 1 * (16*16) + 2 * 16] == 1);
                CHECK(voxels[ 4 + 2 * (16*16) + 10* 16] == 2);
                CHECK(voxels[ 5 + 9 * (16*16) + 4 * 16] == 3);
            }
        }
    }
}

TEST_CASE("solidData", "[solidData]")
{
	VoxelTypeArray voxels;
	Chunk chunk;

	voxels.fill(0);
	chunk.setVoxelData(voxels);
	REQUIRE(chunk.getSolidity() == EMPTY);

	voxels.fill(1);
	voxels[5] = 3;
	chunk.setVoxelData(voxels);
	REQUIRE(chunk.getSolidity() == SOLID);

	voxels[200] = 0;
	chunk.setVoxelData(voxels);
	REQUIRE(chunk.getSolidity() == INBETWEEN);
}

TEST_CASE("solidType", "[solidType]")
{
	VoxelTypeArray voxels;
	Chunk chunk;

	ChunkVoxelCoord coords;
	coords.x = 1;
	coords.y = 0;
	coords.z = 0;

	voxels.fill(0);
	voxels[1] = 1;
	chunk.setVoxelData(voxels);
	chunk.setVoxelType(coords, 0);
	REQUIRE(chunk.getSolidity() == EMPTY);

	voxels.fill(1);
	voxels[1] = 0;
	chunk.setVoxelData(voxels);
	chunk.setVoxelType(coords, 1);
	REQUIRE(chunk.getSolidity() == SOLID);

	chunk.setVoxelType(coords, 0);
	REQUIRE(chunk.getSolidity() == INBETWEEN);
}
