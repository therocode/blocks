#include "../catch.hpp"
#include "../../src/world/explorationmanager.hpp"
#include "../scopeddirectory.hpp"

TEST_CASE("exman", "[exman]")
{
    ScopedDirectory testDir("test_exploration");

	ExplorationManager exman(testDir.getPath());

	ChunkCoord coords(0,0,0);
	REQUIRE(exman.getChunkExplored(coords) == false);
	
	exman.setChunkExplored(coords);
	REQUIRE(exman.getChunkExplored(coords) == true);

	coords = ChunkCoord(1,0,0);
	REQUIRE(exman.getChunkExplored(coords) == false);
	
	coords = ChunkCoord(32,0,0); //make sure 0,0,0 in the next region is empty
	REQUIRE(exman.getChunkExplored(coords) == false);
	
	exman.setChunkExplored(coords);
	REQUIRE(exman.getChunkExplored(coords) == true);

	coords = ChunkCoord(31,31,31); //explore the end of a region to make sure it is saved
	exman.setChunkExplored(coords);
	
	exman.saveExploration();
	
	ExplorationManager exman2(testDir.getPath());
	
	coords = ChunkCoord(0,0,0);
	REQUIRE(exman2.getChunkExplored(coords) == true);
	
	coords = ChunkCoord(1,0,0);
	REQUIRE(exman2.getChunkExplored(coords) == false);
	
	coords = ChunkCoord(32,0,0);
	REQUIRE(exman2.getChunkExplored(coords) == true);
	
	coords = ChunkCoord(31,31,31);
	REQUIRE(exman2.getChunkExplored(coords));
}
