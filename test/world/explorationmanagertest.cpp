#include "../catch.hpp"

#include "../src/world/explorationmanager.hpp"

TEST_CASE("exman", "[exman]")
{
	ExplorationManager exman;
	exman.setWorldName("default");

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
	
	
	ExplorationManager exman2;
	exman2.setWorldName("default");
	
	coords = ChunkCoord(0,0,0);
	REQUIRE(exman2.getChunkExplored(coords) == true);
	
	coords = ChunkCoord(1,0,0);
	REQUIRE(exman2.getChunkExplored(coords) == false);
	
	coords = ChunkCoord(32,0,0);
	REQUIRE(exman2.getChunkExplored(coords) == true);
}
