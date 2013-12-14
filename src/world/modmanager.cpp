#include "modmanager.h"

using namespace std;

const string regionDir = ".";

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

bool VoxelCoordinate_uint8::operator==(const VoxelCoordinate_uint8& other) const
{
    return x == other.x && y == other.y && z == other.z;
}

bool VoxelCoordinate_uint8::operator!=(const VoxelCoordinate_uint8& other) const
{
    return x != other.x || y != other.y || z != other.z;
}

bool VoxelCoordinate_uint8::operator<(const VoxelCoordinate_uint8& other) const
{
    if(x == other.x)
    {
        if(y == other.y)
        {
            return z < other.z;
        }
        else
        {
            return y < other.y;
        }
    } 
    else
    {
        return x < other.x;
    }    
}

ModManager::ModManager(fea::MessageBus& messageBus, RegionCoordinate regionLoc)
    : mBus(messageBus), mRegionLoc(regionLoc)
{
    mIndexPath = regionDir + pathSep + glm::to_string(mRegionLoc) + ".idx";
    mDataPath = regionDir + pathSep + glm::to_string(mRegionLoc) + ".dat";
}

void ModManager::loadMods(const ChunkRegionCoordinate loc)
{
    ChunkIndex chunkIndex = getChunkIndex(loc);

    mMods[loc] = ChunkModMap();
    if(chunkIndex != NO_CHUNK) { 
        ifstream dataFile(mDataPath, ios::in | ios::binary);
        dataFile.seekg(chunkIndex);

        uint16_t modCount;
        dataFile.read((char*)&modCount, sizeof(uint16_t));    

        uint64_t timestamp;
        dataFile.read((char*)&timestamp, sizeof(uint64_t));

        for(int i = 0; i < modCount; ++i) 
        {
            Mod mod;
            dataFile.read((char*)&mod, sizeof(Mod));
            _setMod(loc, mod.coord, mod.type);
        }
        dataFile.close();

        mBus.sendMessage<ChunkLoadedMessage>(ChunkLoadedMessage(loc.toWorldSpace(mRegionLoc), timestamp));
    }
}

void ModManager::applyMods(Chunk& chunk) 
{
    throw ModManagerException("Not yet implemented.");
    /*
    ChunkModMap mods = mMods[chunk.getLocation()]; // <- This won't work if chunk returns ChunkCoordinate and not ChunkRegionCoordinate.
    for(ChunkModMap::iterator it = mods.begin(); it != mods.end(); ++it) 
    {
        chunk.setVoxelType(it->first.x, it->first.y, it->first.z, it->second);
    }
    */
}

void ModManager::saveMods(uint64_t currentTimestamp) 
{
    hash<ChunkRegionCoordinate> crcHash;
    ifstream iIndexFile(mIndexPath, ios::in | ios::binary);

    TimestampMap oldTimestamps;

    if(iIndexFile)
    {
        ifstream iDataFile(mDataPath, ios::in | ios::binary);
        
        if(iDataFile)
        {
            for(int x = 0; x < regionWidth; ++x) 
            {
                for(int y = 0; y < regionWidth; ++y)
                {
                    for(int z = 0; z < regionWidth; ++z)
                    {
                        ChunkRegionCoordinate loc(x, y, z);

                        iIndexFile.seekg(crcHash(loc)*sizeof(ChunkIndex));
                        ChunkIndex chunkIndex;
                        iIndexFile.read((char*)&chunkIndex, sizeof(ChunkIndex));

                        if(chunkIndex != NO_CHUNK)
                        {
                            iDataFile.seekg(chunkIndex);
                            uint16_t modCount;
                            iDataFile.read((char*)(&modCount), sizeof(uint16_t));
                            uint64_t timestamp;
                            iDataFile.read((char*)(&timestamp), sizeof(uint64_t));
                            oldTimestamps[loc] = timestamp;
                            mMods[loc] = ChunkModMap();
                            for(int i = 0; i < modCount; ++i)
                            {
                                Mod mod;
                                iDataFile.read((char*)&mod, sizeof(Mod));
                                _setMod(loc, mod.coord, mod.type);
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

    using it_type = RegionModMap::iterator;
    for(it_type it = mMods.begin(); it != mMods.end(); ++it)
    {

        oIndexFile.seekp(crcHash(it->first)*sizeof(ChunkIndex));
        ChunkIndex index = oDataFile.tellp();
        oIndexFile.write(reinterpret_cast<const char*>(&index), sizeof(ChunkIndex));

        uint16_t modCount = it->second.size();
        oDataFile.write(reinterpret_cast<const char*>(&modCount), sizeof(modCount));

        uint64_t timestamp;
        TimestampMap::const_iterator got = oldTimestamps.find(it->first);
        if(got == oldTimestamps.end())
        {
            timestamp = currentTimestamp;
        }
        else
        {
            timestamp = oldTimestamps[it->first];
        }
        oDataFile.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));

        for(ChunkModMap::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
        {
            Mod mod(it2->first, it2->second);
            oDataFile.write(reinterpret_cast<const char*>(&mod), sizeof(Mod)); 
        }
    }

    oIndexFile.close();
    oDataFile.close();
}

void ModManager::setMod(ChunkRegionCoordinate chunkLoc, VoxelCoordinate_uint8 voxLoc, VoxelType type)
{
    _setMod(chunkLoc, voxLoc, type); 
}

void ModManager::setMod(ChunkRegionCoordinate chunkLoc, VoxelCoordinate voxLoc, VoxelType type)
{
    _setMod(chunkLoc, VoxelCoordinate_uint8(voxLoc), type);
}

VoxelType ModManager::getMod(ChunkRegionCoordinate chunkLoc, VoxelCoordinate voxLoc)
{
    return mMods[chunkLoc][voxLoc];
}

ChunkIndex ModManager::getChunkIndex(ChunkRegionCoordinate loc)
{
    hash<ChunkRegionCoordinate> crcHash;
    ifstream indexFile(mIndexPath, ios::in | ios::binary);

    if(indexFile)
    {
        indexFile.seekg(crcHash(loc)*sizeof(ChunkIndex));

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
    for(int i = 0; i < regionWidth*regionWidth*regionWidth; ++i)
    {
        indexFile.write(reinterpret_cast<const char*>(&index), sizeof(ChunkIndex));
    }

    indexFile.close();
}

void ModManager::_setMod(ChunkRegionCoordinate chunkLoc, VoxelCoordinate_uint8 voxLoc, VoxelType type)
{
    using it_type = RegionModMap::const_iterator;
    it_type it = mMods.find(chunkLoc);
    if(it == mMods.end())
    {
        mMods[chunkLoc] = ChunkModMap();
    }

    mMods[chunkLoc][voxLoc] = type;
}
