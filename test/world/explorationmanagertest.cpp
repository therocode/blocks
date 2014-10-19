#include "../catch.hpp"
#include "../../src/world/explorationmanager.hpp"
#include "../scopeddirectory.hpp"

TEST_CASE("exman", "[exman]")
{
    ScopedDirectory testDir("worlddata/test_exploration");

	ExplorationManager exman(testDir.getPath());

	ChunkCoord coords(0,0,0);
	REQUIRE(exman.getChunkExplored(coords) == false);
	
	exman.setChunkExplored(coords);
	REQUIRE(exman.getChunkExplored(coords) == true);

	coords = ChunkCoord(1,0,0);
	REQUIRE(exman.getChunkExplored(coords) == false);
	
	coords = ChunkCoord(32,0,0);
	REQUIRE(exman.getChunkExplored(coords) == false);
	
	exman.setChunkExplored(coords);
	REQUIRE(exman.getChunkExplored(coords) == true);

	exman.saveExploration();
	
	ExplorationManager exman2(testDir.getPath());
	
	coords = ChunkCoord(0,0,0);
	REQUIRE(exman2.getChunkExplored(coords) == true);
	
	coords = ChunkCoord(1,0,0);
	REQUIRE(exman2.getChunkExplored(coords) == false);
	
	coords = ChunkCoord(32,0,0);
	REQUIRE(exman2.getChunkExplored(coords) == true);
}
