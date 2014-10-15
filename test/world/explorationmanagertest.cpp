#include "../catch.hpp"
#include "world/explorationmanager.hpp"
#include "../scopeddirectory.hpp"


TEST_CASE("exman", "[exman]")
{
    //ScopedDirectory("test");

	ExplorationManager exman;
	exman.setWorldName("test");

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
	exman2.setWorldName("test");
	
	coords = ChunkCoord(0,0,0);
	REQUIRE(exman2.getChunkExplored(coords) == true);
	
	coords = ChunkCoord(1,0,0);
	REQUIRE(exman2.getChunkExplored(coords) == false);
	
	coords = ChunkCoord(32,0,0);
	REQUIRE(exman2.getChunkExplored(coords) == true);
	
	std::string expfile = "worlddata/test/0_0.exp";
	std::remove(expfile.c_str());
}
