#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include <cstdio>
#include <fstream>
#include <string>
#include <tuple>

#include <fea/util.hpp>

#include "world/chunk.hpp"
#include "world/modmanager.hpp"
#include "world/worldmessages.hpp"

#define PR(x) std::cerr << #x << " = " << (x) << std::endl;

using namespace std;

bool fexists(string filename)
{
    ifstream file(filename);
    return file;
}

class TestReceiver : 
    public fea::MessageReceiver<ChunkModdedMessage>
{
    public:
        Chunk chunk;
        uint64_t timestamp;

    private:
        virtual void handleMessage(const ChunkModdedMessage& msg)
        {
            chunk = msg.chunk;
            timestamp = msg.timestamp;
        } 
};

TEST_CASE("set and get", "[set][get]")
{
    ChunkCoord loc(12, 356, 9);
    ChunkVoxelCoord voxLoc(9, 8, 7);
    ChunkVoxelCoord voxLocInvalid(16, 16, 16);
    ChunkVoxelCoord voxLocInvalid2(-1, -1, -1);
    VoxelType type = 12;

    fea::MessageBus bus;
    ModManager manager(bus);

    manager.setWorldName("default");

    SECTION("set and get one voxel")
    {
        manager.setMod(loc, voxLoc, type);
        REQUIRE(type == manager.getMod(loc, voxLoc)); 
    }

    SECTION("set invalid voxel location")
    {
        CHECK_THROWS_AS(manager.setMod(loc, voxLocInvalid, type), ModManagerException);
        CHECK_THROWS_AS(manager.setMod(loc, voxLocInvalid2, type), ModManagerException);
    }
}

TEST_CASE("save and load", "[save][load]")
{
    ChunkCoord loc(0, 0, 0);
    ChunkCoord loc2(32, 32, 32);
    RegionCoord regionLoc = chunkToRegion(loc);
    RegionCoord regionLoc2 = chunkToRegion(loc2);
    ChunkVoxelCoord voxLoc(2, 2, 2);    
    ChunkVoxelCoord voxLoc2(1, 1, 1);
    VoxelType defaultType = 0;
    VoxelType type = 1;
    uint64_t timestamp = 123;

    fea::MessageBus bus;
    VoxelTypeArray voxelData;
    voxelData.fill(defaultType);
    Chunk chunk(loc, voxelData);
    Chunk chunk2(loc2, voxelData);
    Chunk chunkClone(loc, voxelData);
    ModManager manager(bus);
    ModManager manager2(bus);
    manager.deleteRegionFile(regionLoc);
    manager.deleteRegionFile(regionLoc2);
    TestReceiver receiver;
    bus.addSubscriber<ChunkModdedMessage>(receiver);

    SECTION("load an untimestamped chunk")
    {
        manager.loadMods(chunk);

        REQUIRE(0 == receiver.timestamp);
    }

    SECTION("save a region containing an untimestamped chunk")
    {
        manager.setMod(loc, voxLoc, type);
        CHECK_THROWS_AS(manager.saveMods(regionLoc), ModManagerException);
    }

    SECTION("chunkmoddedmessage chunk")
    {
        manager.setMod(loc, voxLoc, type);
        manager.recordTimestamp(loc, timestamp);
        manager.saveMods(regionLoc);
        manager2.loadMods(chunk);

        REQUIRE(chunk.getLocation() == receiver.chunk.getLocation()); 
    }

    SECTION("chunkmoddedmessage timestamp")
    {
        manager.recordTimestamp(loc, timestamp);
        manager.loadMods(chunk);

        REQUIRE(timestamp == receiver.timestamp); 
    }

    SECTION("one voxel")
    {
        manager.setMod(loc, voxLoc, type);
        manager.recordTimestamp(loc, timestamp);
        manager.saveMods(regionLoc);
        manager2.loadMods(chunk);

        REQUIRE(type == chunk.getVoxelType(voxLoc));
        REQUIRE(defaultType == chunk.getVoxelType(voxLoc2));
    }

    SECTION("save and load twice")
    {
        manager.setMod(loc, voxLoc, type);
        manager.recordTimestamp(loc, timestamp);
        manager.saveMods(regionLoc);
        manager.loadMods(chunk);
        manager.setMod(loc, voxLoc2, type);
        manager.recordTimestamp(loc, timestamp);
        manager.saveMods(regionLoc);
        manager.loadMods(chunkClone);

        REQUIRE(type == chunkClone.getVoxelType(voxLoc));
        REQUIRE(type == chunkClone.getVoxelType(voxLoc2));
    }

    SECTION("set and then load with same manager instance")
    {
        manager.setMod(loc, voxLoc, type);
        manager.loadMods(chunk);

        REQUIRE(type == chunk.getVoxelType(voxLoc));
        REQUIRE(defaultType == chunk.getVoxelType(voxLoc2));
    }

    SECTION("set and then load with different manager instance")
    {
        manager.setMod(loc, voxLoc, type);
        manager2.loadMods(chunk);

        REQUIRE(defaultType == chunk.getVoxelType(voxLoc));
        REQUIRE(defaultType == chunk.getVoxelType(voxLoc2)); 
    }

    SECTION("set and then load on a different manager")
    {
        manager.setMod(loc, voxLoc, type);
        manager2.loadMods(chunk);

        REQUIRE(defaultType == chunk.getVoxelType(voxLoc));
        REQUIRE(defaultType == chunk.getVoxelType(voxLoc2));
    }

    SECTION("all regions")
    {
        manager.setMod(loc, voxLoc, type);
        manager.setMod(loc2, voxLoc, type);
        manager.recordTimestamp(loc, timestamp);
        manager.recordTimestamp(loc2, timestamp);
        manager.saveMods();
        manager2.loadMods(chunk);
        manager2.loadMods(chunk2);

        REQUIRE(type == chunk.getVoxelType(voxLoc));
        REQUIRE(defaultType == chunk.getVoxelType(voxLoc2));
        REQUIRE(type == chunk2.getVoxelType(voxLoc));
        REQUIRE(defaultType == chunk.getVoxelType(voxLoc2));
    }
}
