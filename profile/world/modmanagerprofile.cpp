#include <iostream>
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

int main()
{
    string regionName = "testRegion";
    string indexPath = regionDir + pathSep + regionName + ".idx";
    string dataPath = regionDir + pathSep + regionName + ".dat";

    if(fexists(indexPath))
    {
        remove(indexPath.c_str());
    }

    if(fexists(dataPath))
    {
        remove(dataPath.c_str());
    }

    VoxelType type1 = 1;

    ModManager manager(regionName);

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
    for(int cx = 0; cx < regionWidth; ++cx)
    {
        for(int cy = 0; cy < 1; ++cy)
        {
            for(int cz = 0; cz < 1; ++cz)
            {
                ChunkRegionCoordinate chunkLoc(cx, cy, cz);
                manager2.loadMods(chunkLoc);
            }
        }
    }
}


