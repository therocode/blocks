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

const string pathSep =
#ifdef _WIN32
    "\\";
#else
    "/";
#endif

bool fexists(string filename)
{
    ifstream file(filename);
    return file;
}

class ChunkLoadedReceiver : public fea::MessageReceiver<ChunkLoadedMessage>
{
    public:
        uint64_t getTimestamp()
        {
            return mTimestamp;
        }

    private:
        void handleMessage(const ChunkLoadedMessage& message)
        {
            tie(ignore, mTimestamp) = message.data; 
        }

        uint64_t mTimestamp;
};

TEST_CASE("save and load one", "[save][load]")
{
    fea::MessageBus bus;
    RegionCoordinate regionLoc(-1,-1,-1);
    uint64_t timestamp = 0;

    string indexPath = regionDir + pathSep + glm::to_string(regionLoc) + ".idx";
    string dataPath = regionDir + pathSep + glm::to_string(regionLoc) + ".dat";
    if(fexists(indexPath))
    {
        remove(indexPath.c_str());
    }

    if(fexists(dataPath))
    {
        remove(dataPath.c_str());
    }

    ChunkRegionCoordinate chunkLoc1 = ChunkRegionCoordinate(1, 1, 1);
    VoxelCoordinate voxLoc1 = VoxelCoordinate(1, 1, 1);
    VoxelType type1 = 1;

    ModManager manager(bus, regionLoc);
    manager.setMod(chunkLoc1, voxLoc1, type1);
    manager.saveMods(timestamp);

    ModManager manager2(bus, regionLoc);
    manager2.loadMods(chunkLoc1);
    REQUIRE(type1 == manager2.getMod(chunkLoc1, voxLoc1)); 
}

TEST_CASE("save and load many", "[save][load]")
{
    fea::MessageBus bus;
    RegionCoordinate regionLoc(1,1,1);
    uint64_t timestamp = 0;

    string indexPath = regionDir + pathSep + glm::to_string(regionLoc) + ".idx";
    string dataPath = regionDir + pathSep + glm::to_string(regionLoc) + ".dat";
    if(fexists(indexPath))
    {
        remove(indexPath.c_str());
    }

    if(fexists(dataPath))
    {
        remove(dataPath.c_str());
    }

    VoxelType type1 = 1;

    ModManager manager(bus, regionLoc);

    for(int cx = 0; cx < regionWidth; ++cx)
    {
        for(int cy = 0; cy < 1; ++cy)
        {
            for(int cz = 0; cz < 1; ++cz)
            {
                ChunkRegionCoordinate chunkLoc(cx, cy, cz);

                for(int vx = 0; vx < chunkWidth; ++vx)
                {
                    for(int vy = 0; vy < chunkWidth; ++vy)
                    {
                        for(int vz = 0; vz < chunkWidth; ++vz)
                        {
                            VoxelCoordinate voxLoc(vx, vy, vz);
                            manager.setMod(chunkLoc, voxLoc, type1);
                        }
                    }
                }

            }
        }
    }

    manager.saveMods(timestamp);

    ModManager manager2(bus, regionLoc);
    for(int cx = 0; cx < regionWidth; ++cx)
    {
        for(int cy = 0; cy < 1; ++cy)
        {
            for(int cz = 0; cz < 1; ++cz)
            {
                ChunkRegionCoordinate chunkLoc(cx, cy, cz);
                manager2.loadMods(chunkLoc);

                for(int vx = 0; vx < chunkWidth; ++vx)
                {
                    for(int vy = 0; vy < chunkWidth; ++vy)
                    {
                        for(int vz = 0; vz < chunkWidth; ++vz)
                        {
                            VoxelCoordinate voxLoc(vx, vy, vz);
                            REQUIRE(type1 == manager2.getMod(chunkLoc, voxLoc)); 
                        }
                    }
                }
            }
        }
    }
}

TEST_CASE("save and load all", "[save][load]")
{
    fea::MessageBus bus;
    RegionCoordinate regionLoc(1,1,1);
    uint64_t timestamp = 0;

    string indexPath = regionDir + pathSep + glm::to_string(regionLoc) + ".idx";
    string dataPath = regionDir + pathSep + glm::to_string(regionLoc) + ".dat";
    if(fexists(indexPath))
    {
        remove(indexPath.c_str());
    }

    if(fexists(dataPath))
    {
        remove(dataPath.c_str());
    }

    VoxelType type1 = 1;

    ModManager manager(bus, regionLoc);

    for(int cx = 0; cx < regionWidth; ++cx)
    {
        for(int cy = 0; cy < regionWidth; ++cy)
        {
            for(int cz = 0; cz < regionWidth; ++cz)
            {
                ChunkRegionCoordinate chunkLoc(cx, cy, cz);

                for(int vx = 0; vx < chunkWidth; ++vx)
                {
                    for(int vy = 0; vy < chunkWidth; ++vy)
                    {
                        for(int vz = 0; vz < chunkWidth; ++vz)
                        {
                            VoxelCoordinate voxLoc(vx, vy, vz);

                            manager.setMod(chunkLoc, voxLoc, type1);
                        }
                    }
                }
            }
        }
    }

    manager.saveMods(timestamp);

    ModManager manager2(bus, regionLoc);
    for(int cx = 0; cx < regionWidth; ++cx)
    {
        for(int cy = 0; cy < regionWidth; ++cy)
        {
            for(int cz = 0; cz < regionWidth; ++cz)
            {
                ChunkRegionCoordinate chunkLoc(cx, cy, cz);
                manager2.loadMods(chunkLoc);

                for(int vx = 0; vx < chunkWidth; ++vx)
                {
                    for(int vy = 0; vy < chunkWidth; ++vy)
                    {
                        for(int vz = 0; vz < chunkWidth; ++vz)
                        {
                            VoxelCoordinate voxLoc(vx, vy, vz);
                            REQUIRE(type1 == manager2.getMod(chunkLoc, voxLoc)); 
                        }
                    }
                }
            }
        }
    }
}

TEST_CASE("timestamp once", "[timestamp]")
{
    fea::MessageBus bus;
    ChunkLoadedReceiver receiver;
    bus.addMessageSubscriber(receiver);

    RegionCoordinate regionLoc(1, 1, 1);
    ChunkRegionCoordinate chunkLoc(1, 1, 1);
    VoxelCoordinate voxLoc(1, 1, 1);
    VoxelType type = 1;
    uint64_t timestamp = 643;

    string indexPath = regionDir + pathSep + glm::to_string(regionLoc) + ".idx";
    string dataPath = regionDir + pathSep + glm::to_string(regionLoc) + ".dat";
    if(fexists(indexPath))
    {
        remove(indexPath.c_str());
    }

    if(fexists(dataPath))
    {
        remove(dataPath.c_str());
    }

    ModManager manager1(bus, regionLoc);
    manager1.setMod(chunkLoc, voxLoc, type);
    manager1.saveMods(timestamp);

    ModManager manager2(bus, regionLoc);
    manager2.loadMods(chunkLoc);

    REQUIRE(timestamp == receiver.getTimestamp()); 
}

TEST_CASE("timestamp twice", "[timestamp]")
{
    fea::MessageBus bus;
    ChunkLoadedReceiver receiver;
    bus.addMessageSubscriber(receiver);

    RegionCoordinate regionLoc(1, 1, 1);
    VoxelCoordinate voxLoc(1, 1, 1);
    VoxelType type = 1;
    ModManager manager1(bus, regionLoc);
    ModManager manager2(bus, regionLoc);

    string indexPath = regionDir + pathSep + glm::to_string(regionLoc) + ".idx";
    string dataPath = regionDir + pathSep + glm::to_string(regionLoc) + ".dat";
    if(fexists(indexPath))
    {
        remove(indexPath.c_str());
    }

    if(fexists(dataPath))
    {
        remove(dataPath.c_str());
    }

    ChunkRegionCoordinate chunkLoc(1, 1, 1);
    uint64_t timestamp = 643;

    manager1.setMod(chunkLoc, voxLoc, type);
    manager1.saveMods(timestamp);

    ChunkRegionCoordinate chunkLoc2(2,2,2);
    uint64_t timestamp2 = 12312;

    manager1.setMod(chunkLoc2, voxLoc, type);
    manager1.saveMods(timestamp2);

    manager2.loadMods(chunkLoc);

    REQUIRE(timestamp == receiver.getTimestamp());

    manager2.loadMods(chunkLoc2);

    REQUIRE(timestamp2 == receiver.getTimestamp());
}
