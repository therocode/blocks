#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include "world/chunk.hpp"

TEST_CASE("solid", "[solid]")
{
	VoxelTypeArray voxels;
	Chunk chunk;

	for(auto &v : voxels)
		v = 0;
	chunk.setVoxelData(voxels);
	REQUIRE(chunk.getSolidity() == EMPTY);

	for(auto &v : voxels)
		v = rand() | 1;
	chunk.setVoxelData(voxels);
	REQUIRE(chunk.getSolidity() == SOLID);

	for(auto &v : voxels)
		v = rand();
	voxels[4] = 1;
	voxels[200] = 0;
	chunk.setVoxelData(voxels);
	REQUIRE(chunk.getSolidity() == INBETWEEN);
}
