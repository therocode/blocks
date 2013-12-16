#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include <cstdio>
#include <fstream>
#include <string>

#include <featherkit/messaging.h>

#include <glm/gtx/string_cast.hpp>

#include "world/chunk.h"
#include "world/modmanager.h"

#define PR(x) std::cerr << #x << " = " << (x) << std::endl;

using namespace std;

bool fexists(string filename)
{
    ifstream file(filename);
    return file;
}

TEST_CASE("set and get", "[set][get]")
{
    ChunkCoordinate loc(12, 356, 9);
    VoxelCoordinate voxLoc(9, 8, 7);
    VoxelCoordinate voxLocInvalid(16, 16, 16);
    VoxelCoordinate voxLocInvalid2(-1, -1, -1);
    VoxelType type = 12;

    ModManager manager;

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
    ChunkCoordinate loc(0, 0, 0);
    ChunkCoordinate loc2(32, 32, 32);
    RegionCoordinate regionLoc = chunkToRegion(loc);
    RegionCoordinate regionLoc2 = chunkToRegion(loc2);
    VoxelCoordinate voxLoc(0, 0, 0);    
    VoxelCoordinate voxLoc2(1, 1, 1);
    VoxelType defaultType = 0;
    VoxelType type = 1;
    uint64_t timestamp = 0;

    VoxelTypeArray voxelData;
    voxelData.fill(defaultType);
    Chunk chunk(loc, voxelData);
    Chunk chunk2(loc2, voxelData);
    ModManager manager;
    ModManager manager2;
    manager.deleteRegionFile(regionLoc);
    manager.deleteRegionFile(regionLoc2);

    SECTION("one voxel")
    {
        manager.setMod(loc, voxLoc, type);
        manager.saveMods(timestamp, regionLoc);
        manager2.loadMods(chunk);

        REQUIRE(type == chunk.getVoxelType(voxLoc));
        REQUIRE(defaultType == chunk.getVoxelType(voxLoc2));
    }

    SECTION("set and load but don't save")
    {
        manager.setMod(loc, voxLoc, type);
        manager.loadMods(chunk);

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
        manager.saveMods(timestamp);
        manager.loadMods(chunk);
        manager.loadMods(chunk2);

        REQUIRE(type == chunk.getVoxelType(voxLoc));
        REQUIRE(defaultType == chunk.getVoxelType(voxLoc2));
        REQUIRE(type == chunk2.getVoxelType(voxLoc));
        REQUIRE(defaultType == chunk.getVoxelType(voxLoc2));
    }
}

