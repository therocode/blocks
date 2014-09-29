#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include "world/chunk.hpp"

TEST_CASE("solid", "[solid]")
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
