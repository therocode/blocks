#include "../catch.hpp"
#include "../scopeddirectory.hpp"
#include "../src/world/chunk.hpp"
#include "../src/world/modmanager.hpp"
#include "../src/world/worldmessages.hpp"

TEST_CASE("set and get", "[set][get]")
{
    ScopedDirectory testDir("test_directory");
    VoxelCoord voxLoc(9, 8, 7);
    VoxelType type = 12;
    ChunkCoord loc = VoxelToChunk::convert(voxLoc);
    uint64_t timestamp = 123;

    ModManager manager("test_directory");

    SECTION("set and get one voxel")
    {
        manager.setMod(voxLoc, type);
        REQUIRE(type == manager.getMod(voxLoc)); 
    }

    SECTION("get a timestamp for chunk in a completely clean modmanager and mod directory") 
	{
		REQUIRE(0 == manager.getTimestamp(loc));
	}
    
    SECTION("set and get a timestamp")
	{
		manager.setTimestamp(loc, timestamp);
		REQUIRE(timestamp == manager.getTimestamp(loc));
	}
	
}

TEST_CASE("save and load", "[save][load]")
{
    ScopedDirectory testDir("test_directory");
    VoxelCoord voxLoc(2, 2, 2);    
    VoxelCoord voxLoc2(580, 580, 580);
    ChunkCoord loc = VoxelToChunk::convert(voxLoc);
    ChunkCoord loc2 = VoxelToChunk::convert(voxLoc2);
    ModRegionCoord regionLoc = ChunkToModRegion::convert(loc);
    ModRegionCoord regionLoc2 = ChunkToModRegion::convert(loc2);
    VoxelType defaultType = 0;
    VoxelType type = 1;
    uint64_t timestamp = 123;

    VoxelTypeArray voxelData;
    voxelData.fill(defaultType);
    Chunk chunk(voxelData);
    Chunk chunk2(voxelData);
    Chunk chunkClone(voxelData);
    ModManager manager("test_directory");
    ModManager manager2("test_directory");

    SECTION("load an untimestamped chunk")
    {
        manager.loadMods(loc, chunk);

        REQUIRE(0 == manager.getTimestamp(loc));
    }

    SECTION("one voxel")
    {
        manager.setMod(voxLoc, type);
        manager.setTimestamp(loc, timestamp);

        REQUIRE_FALSE(manager2.hasMods(loc));

        manager.saveMods(regionLoc);

        REQUIRE(manager2.hasMods(loc));

        manager2.loadMods(loc, chunk);

        REQUIRE(type == chunk.getVoxelType(VoxelToChunkVoxel::convert(voxLoc)));
        REQUIRE(defaultType == chunk.getVoxelType(VoxelToChunkVoxel::convert(voxLoc2)));
        REQUIRE(manager.hasMods(loc));
        REQUIRE(manager2.hasMods(loc));
        REQUIRE_FALSE(manager.hasMods(loc + ChunkCoord(2, 5, 4)));
        REQUIRE_FALSE(manager2.hasMods(loc + ChunkCoord(1, 7, 4)));
    }

    SECTION("save and load twice")
    {
        manager.setMod(voxLoc, type);
        manager.setTimestamp(loc, timestamp);
        manager.saveMods(regionLoc);
        manager.loadMods(loc, chunk);
        manager.setMod(voxLoc + VoxelCoord(1, 1, 1), type);
        manager.setTimestamp(loc, timestamp);
        manager.saveMods(regionLoc);
        manager.loadMods(loc, chunkClone);

        REQUIRE(type == chunkClone.getVoxelType(VoxelToChunkVoxel::convert(voxLoc)));
        REQUIRE(type == chunkClone.getVoxelType(VoxelToChunkVoxel::convert(voxLoc + VoxelCoord(1, 1, 1))));
    }

    SECTION("set and then load with same manager instance")
    {
        manager.setMod(voxLoc, type);
        manager.loadMods(loc, chunk);

        REQUIRE(type == chunk.getVoxelType(VoxelToChunkVoxel::convert(voxLoc)));
        REQUIRE(defaultType == chunk.getVoxelType(VoxelToChunkVoxel::convert(voxLoc2)));
    }

    SECTION("set and then load with different manager instance")
    {
        manager.setMod(voxLoc, type);
        manager2.loadMods(loc, chunk);

        REQUIRE(defaultType == chunk.getVoxelType(VoxelToChunkVoxel::convert(voxLoc)));
        REQUIRE(defaultType == chunk.getVoxelType(VoxelToChunkVoxel::convert(voxLoc2))); 
    }

    SECTION("set and then load on a different manager")
    {
        manager.setMod(voxLoc, type);
        manager2.loadMods(loc, chunk);

        REQUIRE(defaultType == chunk.getVoxelType(VoxelToChunkVoxel::convert(voxLoc)));
        REQUIRE(defaultType == chunk.getVoxelType(VoxelToChunkVoxel::convert(voxLoc2)));
    }

    SECTION("all regions")
    {
        manager.setMod(voxLoc, type);
        manager.setMod(voxLoc2, type);
        manager.setTimestamp(loc, timestamp);
        manager.setTimestamp(loc2, timestamp);
        manager.saveMods();
        manager2.loadMods(loc, chunk);
        manager2.loadMods(loc2, chunk2);

        REQUIRE(type == chunk.getVoxelType(VoxelToChunkVoxel::convert(voxLoc)));
        REQUIRE(defaultType == chunk.getVoxelType({15, 14, 13}));
        REQUIRE(type == chunk2.getVoxelType(VoxelToChunkVoxel::convert(voxLoc2)));
        REQUIRE(defaultType == chunk.getVoxelType({15, 14, 13}));
    }
}
