#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include "world/chunk.hpp"

using ChunkVoxelCoord  = glm::u8vec3;

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
	
	ChunkVoxelCoord coords(1, 0, 0);

	voxels.fill(0);
	chunk.setVoxelData(voxels);

	chunk.setVoxelType(coords, 1);
	REQUIRE(chunk.getSolidity() == INBETWEEN);

	chunk.setVoxelType(coords, 0);
	REQUIRE(chunk.getSolidity() == EMPTY);

	voxels.fill(1);
	chunk.setVoxelData(voxels);

	chunk.setVoxelType(coords, 0);
	REQUIRE(chunk.getSolidity() == INBETWEEN);

	chunk.setVoxelType(coords, 1);
	REQUIRE(chunk.getSolidity() == SOLID);
}
