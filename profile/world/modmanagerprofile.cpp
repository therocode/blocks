#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#include <glm/gtx/string_cast.hpp>

#include <featherkit/messaging.h>

#include "world/chunk.h"
#include "world/modmanager.h"

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

int main()
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

    cout << "Setting..." << endl;

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
                            VoxelChunkCoordinate voxLoc(vx, vy, vz);
                            manager.setMod(chunkLoc, voxLoc, type1);
                        }
                    }
                }

            }
        }
    }

    cout << "Saving..." << endl;

    manager.saveMods(timestamp);

    cout << "Loading..." << endl;

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
                            VoxelChunkCoordinate voxLoc(vx, vy, vz);
                            manager.getMod(chunkLoc, voxLoc);
                        }
                    }
                }
            }
        }
    }
}


