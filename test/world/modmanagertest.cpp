#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include <cstdio>
#include <fstream>
#include <string>

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

TEST_CASE("save and load", "[save][load]")
{
    string regionName = "testRegion";
    string indexPath = REGION_DIR + pathSep + regionName + ".idx";
    string dataPath = REGION_DIR + pathSep + regionName + ".dat";

    if(fexists(indexPath))
    {
        remove(indexPath.c_str());
    }

    if(fexists(dataPath))
    {
        remove(dataPath.c_str());
    }

    ChunkCoordinate chunkLoc1 = ChunkCoordinate(1, 1, 1);
    VoxelCoordinate voxLoc1 = VoxelCoordinate(1, 1, 1);
    VoxelType type1 = 1;

    SECTION("set single mod")
    {
        ModManager manager(regionName);
        manager.setMod(chunkLoc1, voxLoc1, type1);
        manager.saveMods();

        ModManager manager2(regionName);
        manager2.loadMods(chunkLoc1);
        REQUIRE(type1 == manager2.getMod(chunkLoc1, voxLoc1)); 
    }

    SECTION("set some mods")
    {
        ModManager manager(regionName);

        cout << "Setting..." << endl;

        for(int cx = 0; cx < REGION_LENGTH; ++cx)
        {
            for(int cy = 0; cy < REGION_LENGTH; ++cy)
            {
                for(int cz = 0; cz < 1; ++cz)
                {
                    ChunkCoordinate chunkLoc(cx, cy, cz);

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

        cout << "Saving..." << endl;

        manager.saveMods();

        cout << "Loading..." << endl;

        ModManager manager2(regionName);
        for(int cx = 0; cx < REGION_LENGTH; ++cx)
        {
            for(int cy = 0; cy < REGION_LENGTH; ++cy)
            {
                for(int cz = 0; cz < 1; ++cz)
                {
                    ChunkCoordinate chunkLoc(cx, cy, cz);
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

    /*
    SECTION("set all mods")
    {
        ModManager manager(regionName);

        cout << "Setting..." << endl;

        for(int cx = 0; cx < REGION_LENGTH; ++cx)
        {
            for(int cy = 0; cy < REGION_LENGTH; ++cy)
            {
                for(int cz = 0; cz < REGION_LENGTH; ++cz)
                {
                    ChunkCoordinate chunkLoc(cx, cy, cz);

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

        cout << "Saving..." << endl;

        manager.saveMods();

        cout << "Loading..." << endl;

        ModManager manager2(regionName);
        for(int cx = 0; cx < REGION_LENGTH; ++cx)
        {
            for(int cy = 0; cy < REGION_LENGTH; ++cy)
            {
                for(int cz = 0; cz < REGION_LENGTH; ++cz)
                {
                    ChunkCoordinate chunkLoc(cx, cy, cz);
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
    */
}
