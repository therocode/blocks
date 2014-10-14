#include "../catch.hpp"

#include <fstream>

#include "../src/world/chunk.hpp"
#include "../src/world/modmanager.hpp"
#include "../src/world/worldmessages.hpp"

bool fexists(std::string filename)
{
    std::ifstream file(filename);
    return file;
}

TEST_CASE("set and get", "[set][get]")
{
    VoxelCoord voxLoc(9, 8, 7);
    VoxelType type = 12;

    ModManager manager("test");

    SECTION("set and get one voxel")
    {
        manager.setMod(voxLoc, type);
        REQUIRE(type == manager.getMod(voxLoc)); 
    }
}

TEST_CASE("save and load", "[save][load]")
{
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
    ModManager manager("test");
    ModManager manager2("test");
    manager.deleteModRegionFile(regionLoc);
    manager.deleteModRegionFile(regionLoc2);

    //SECTION("load an untimestamped chunk")   //this test might need to me reintroduced, issue #133
    //{
    //    manager.loadMods(chunk);

    //    REQUIRE(0 == receiver.timestamp);
    //}

    //SECTION("save a region containing an untimestamped chunk") //this test might need to me reintroduced, issue #133
    //{
    //    manager.setMod(loc, voxLoc, type);
    //    CHECK_THROWS_AS(manager.saveMods(regionLoc), ModManagerException);
    //}

    //SECTION("chunkmoddedmessage timestamp") //this test might need to me reintroduced, issue #133
    //{
    //    manager.recordTimestamp(loc, timestamp);
    //    manager.loadMods(chunk);

    //    REQUIRE(timestamp == receiver.timestamp); 
    //}

    SECTION("one voxel")
    {
        manager.setMod(voxLoc, type);
        manager.recordTimestamp(loc, timestamp);
        manager.saveMods(regionLoc);
        manager2.loadMods(loc, chunk);

        REQUIRE(type == chunk.getVoxelType(VoxelToChunkVoxel::convert(voxLoc)));
        REQUIRE(defaultType == chunk.getVoxelType(VoxelToChunkVoxel::convert(voxLoc2)));
    }

    SECTION("save and load twice")
    {
        manager.setMod(voxLoc, type);
        manager.recordTimestamp(loc, timestamp);
        manager.saveMods(regionLoc);
        manager.loadMods(loc, chunk);
        manager.setMod(voxLoc + VoxelCoord(1, 1, 1), type);
        manager.recordTimestamp(loc, timestamp);
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
        manager.recordTimestamp(loc, timestamp);
        manager.recordTimestamp(loc2, timestamp);
        manager.saveMods();
        manager2.loadMods(loc, chunk);
        manager2.loadMods(loc2, chunk2);

        REQUIRE(type == chunk.getVoxelType(VoxelToChunkVoxel::convert(voxLoc)));
        REQUIRE(defaultType == chunk.getVoxelType({15, 14, 13}));
        REQUIRE(type == chunk2.getVoxelType(VoxelToChunkVoxel::convert(voxLoc2)));
        REQUIRE(defaultType == chunk.getVoxelType({15, 14, 13}));
    }
}
