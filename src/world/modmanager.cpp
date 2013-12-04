#include "modmanager.h"

using namespace std;

const string REGION_DIR = ".";
const uint8_t REGION_LENGTH = 32;

const string pathSep =
#ifdef _WIN32
    "\\";
#else
    "/";
#endif

VoxelCoordinate_uint8::VoxelCoordinate_uint8() {}
VoxelCoordinate_uint8::VoxelCoordinate_uint8(VoxelCoordinate coord)
    : x(coord.x), y(coord.y), z(coord.z) 
{}

ModManager::ModManager(string regionName)
    : mRegionName(regionName)
{
    mIndexPath = REGION_DIR + pathSep + mRegionName + ".idx";
    mDataPath = REGION_DIR + pathSep + mRegionName + ".dat";
}

void ModManager::loadMods(const ChunkCoordinate loc)
{
    ChunkIndex chunkIndex = getChunkIndex(loc);

    mMods[loc] = ChunkModMap();
    if(chunkIndex != NO_CHUNK) { 
        ifstream dataFile(mDataPath, ios::in | ios::binary);
        dataFile.seekg(chunkIndex);

        uint16_t modCount;
        dataFile.read((char*)&modCount, sizeof(uint16_t));    

        for(int i = 0; i < modCount; ++i) 
        {
            Mod mod;
            dataFile.read((char*)&mod, sizeof(Mod));
            setMod(loc, mod.coord.toVoxelCoordinate(), mod.type);
        }
        dataFile.close();
    }
}

void ModManager::applyMods(Chunk& chunk) 
{
    ChunkModMap mods = mMods[chunk.getLocation()];
    for(ChunkModMap::iterator it = mods.begin(); it != mods.end(); ++it) 
    {
        chunk.setVoxelType(it->first.x, it->first.y, it->first.z, it->second);
    }
}

void ModManager::saveMods() 
{
    hash<ChunkCoordinate> chunkHash;
    ifstream iIndexFile(mIndexPath, ios::in | ios::binary);

    if(iIndexFile)
    {
        ifstream iDataFile(mDataPath, ios::in | ios::binary);
        
        if(iDataFile)
        {
            for(int x = 0; x < REGION_LENGTH; ++x) 
            {
                for(int y = 0; y < REGION_LENGTH; ++y)
                {
                    for(int z = 0; z < REGION_LENGTH; ++z)
                    {
                        ChunkCoordinate loc(x, y, z);

                        iIndexFile.seekg(chunkHash(loc));
                        ChunkIndex chunkIndex;
                        iIndexFile.read((char*)&chunkIndex, sizeof(ChunkIndex));

                        if(chunkIndex != NO_CHUNK)
                        {
                            iDataFile.seekg(chunkIndex);
                            uint16_t modCount;
                            iDataFile.read((char*)(&modCount), sizeof(uint16_t));
                            mMods[loc] = ChunkModMap();
                            for(int i = 0; i < modCount; ++i)
                            {
                                Mod mod;
                                iDataFile.read((char*)&mod, sizeof(Mod));
                                setMod(loc, mod.coord.toVoxelCoordinate(), mod.type);
                            } 
                        } 
                    }
                }
            }

            iDataFile.close();
            remove(mDataPath.c_str());
        }

        iIndexFile.close();
        remove(mIndexPath.c_str());
    }
   
    initIndexFile();

    fstream oIndexFile(mIndexPath, ios::binary | ios::in | ios::out);
    oIndexFile.clear();
    ofstream oDataFile(mDataPath, ios::out | ios::binary);

    using it_type = unordered_map<ChunkCoordinate, ChunkModMap>::iterator;
    for(it_type it = mMods.begin(); it != mMods.end(); ++it)
    {

        oIndexFile.seekp(chunkHash(it->first)*sizeof(ChunkIndex));
        ChunkIndex index = oDataFile.tellp();
        oIndexFile.write(reinterpret_cast<const char*>(&index), sizeof(ChunkIndex));

        uint16_t modCount = it->second.size();
        oDataFile.write(reinterpret_cast<const char*>(&modCount), sizeof(modCount));
        for(ChunkModMap::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
        {
            Mod mod(VoxelCoordinate_uint8(it2->first), it2->second);
            oDataFile.write(reinterpret_cast<const char*>(&mod), sizeof(Mod)); 
        }
    }

    oIndexFile.close();
    oDataFile.close();
}

void ModManager::setMod(ChunkCoordinate chunkLoc, VoxelCoordinate voxLoc, VoxelType type)
{
    using it_type = unordered_map<ChunkCoordinate, ChunkModMap>::const_iterator;
    it_type it = mMods.find(chunkLoc);
    if(it == mMods.end())
    {
        mMods[chunkLoc] = ChunkModMap();
    }

    mMods[chunkLoc][voxLoc] = type;
}

VoxelType ModManager::getMod(ChunkCoordinate chunkLoc, VoxelCoordinate voxLoc)
{
    return mMods[chunkLoc][voxLoc];
}

ChunkIndex ModManager::getChunkIndex(ChunkCoordinate loc)
{
    hash<ChunkCoordinate> chunkHash;
    ifstream indexFile(mIndexPath, ios::in | ios::binary);

    if(indexFile)
    {
        indexFile.seekg(chunkHash(loc)*sizeof(ChunkIndex));

        ChunkIndex chunkIndex;
        indexFile.read((char*)&chunkIndex, sizeof(ChunkIndex));
        indexFile.close();

        return chunkIndex;
    } 
    else
    {
        initIndexFile();
        return NO_CHUNK;
    } 
}

void ModManager::initIndexFile()
{
    ofstream indexFile(mIndexPath, ios::out | ios::binary);

    ChunkIndex index = NO_CHUNK;
    for(int x = 0; x < REGION_LENGTH; ++x)
    {
        for(int y = 0; y < REGION_LENGTH; ++y)
        {
            for(int z = 0; z < REGION_LENGTH; ++z)
            {
                ChunkCoordinate loc(x, y, z);
                indexFile.write(reinterpret_cast<const char*>(&index), sizeof(ChunkIndex));
            }
        }
    }

    indexFile.close();
}
