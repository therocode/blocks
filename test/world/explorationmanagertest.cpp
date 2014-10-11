#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include "world/explorationmanager.hpp"



TEST_CASE("exman", "[exman]")
{
	ExplorationManager exman;
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
}
