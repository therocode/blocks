#include "modmanager.h"

using namespace std;

VoxelCoordinate_uint8::VoxelCoordinate_uint8() {}
VoxelCoordinate_uint8::VoxelCoordinate_uint8(ChunkVoxelCoord coord)
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

ModManager::ModManager(fea::MessageBus& bus) 
    : mBus(bus) {}

void ModManager::loadMods(Chunk& chunk)
{
    RegionCoord regionLoc = chunkToRegion(chunk.getLocation());
    RegionChunkCoord chunkLoc = chunkToRegionChunk(chunk.getLocation()); 

    ChunkIndex chunkIndex = getChunkIndex(regionLoc, chunkLoc);

    mMods[regionLoc][chunkLoc] = ChunkModMap();

    uint64_t timestamp;
    if(chunkIndex != NO_CHUNK) { 
        ifstream dataFile(getFilename(regionLoc) + dataExt, ios::in | ios::binary);
        dataFile.seekg(chunkIndex);

        uint16_t modCount;
        dataFile.read((char*)&modCount, sizeof(uint16_t));    

        dataFile.read((char*)&timestamp, sizeof(uint64_t));

        for(int i = 0; i < modCount; ++i) 
        {
            Mod mod;
            dataFile.read((char*)&mod, sizeof(Mod));
            _setMod(regionLoc, chunkLoc, mod.coord, mod.type);
        }
        dataFile.close();
    }

    ChunkModMap mods = mMods[regionLoc][chunkLoc]; 
    for(ChunkModMap::iterator it = mods.begin(); it != mods.end(); ++it) 
    {
        chunk.setVoxelType(it->first.x, it->first.y, it->first.z, it->second);
    }

    mBus.sendMessage<ChunkModdedMessage>(ChunkModdedMessage(chunk, timestamp));
}

void ModManager::saveMods(uint64_t currentTimestamp)
{
    for(WorldModMap::iterator it = mMods.begin(); it != mMods.end(); ++it)
    {
        saveMods(currentTimestamp, it->first);
    }
}

void ModManager::saveMods(uint64_t currentTimestamp, RegionCoord regionLoc) 
{
    string dataFilename = getFilename(regionLoc) + dataExt;
    string indexFilename = getFilename(regionLoc) + indexExt;

    hash<RegionChunkCoord> crcHash;
    ifstream iIndexFile(indexFilename, ios::in | ios::binary);

    TimestampMap oldTimestamps;

    if(iIndexFile)
    {
        ifstream iDataFile(dataFilename, ios::in | ios::binary);
        
        if(iDataFile)
        {
            for(int x = 0; x < regionWidth; ++x) 
            {
                for(int y = 0; y < regionWidth; ++y)
                {
                    for(int z = 0; z < regionWidth; ++z)
                    {
                        RegionChunkCoord chunkLoc(x, y, z);

                        RegionModMap::const_iterator got = mMods[regionLoc].find(chunkLoc);
                        if(got == mMods[regionLoc].end())
                        {
                            iIndexFile.seekg(crcHash(chunkLoc)*sizeof(ChunkIndex));
                            ChunkIndex chunkIndex;
                            iIndexFile.read((char*)&chunkIndex, sizeof(ChunkIndex));

                            if(chunkIndex != NO_CHUNK)
                            {
                                iDataFile.seekg(chunkIndex);
                                uint16_t modCount;
                                iDataFile.read((char*)(&modCount), sizeof(uint16_t));
                                uint64_t timestamp;
                                iDataFile.read((char*)(&timestamp), sizeof(uint64_t));
                                oldTimestamps[chunkLoc] = timestamp;
                                mMods[regionLoc][chunkLoc] = ChunkModMap();
                                for(int i = 0; i < modCount; ++i)
                                {
                                    Mod mod;
                                    iDataFile.read((char*)&mod, sizeof(Mod));
                                    _setMod(regionLoc, chunkLoc, mod.coord, mod.type);
                                } 
                            }
                        } 
                    }
                }
            }

            iDataFile.close();
            remove(dataFilename.c_str());
        }

        iIndexFile.close();
        remove(indexFilename.c_str());
    }
   
    initIndexFile(regionLoc);

    fstream oIndexFile(indexFilename, ios::binary | ios::in | ios::out);
    oIndexFile.clear();
    ofstream oDataFile(dataFilename, ios::out | ios::binary);

    RegionModMap regionMods = mMods[regionLoc];
    for(RegionModMap::iterator it = regionMods.begin(); it != regionMods.end(); ++it)
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

void ModManager::setMod(ChunkCoord loc, VoxelCoordinate_uint8 voxLoc, VoxelType type)
{
    RegionCoord regionLoc = chunkToRegion(loc);
    RegionChunkCoord chunkLoc = chunkToRegionChunk(loc);

    _setMod(regionLoc, chunkLoc, voxLoc, type); 
}

void ModManager::setMod(ChunkCoord loc, ChunkVoxelCoord voxLoc, VoxelType type)
{
    RegionCoord regionLoc = chunkToRegion(loc);
    RegionChunkCoord chunkLoc = chunkToRegionChunk(loc);

    _setMod(regionLoc, chunkLoc, VoxelCoordinate_uint8(voxLoc), type);
}

VoxelType ModManager::getMod(ChunkCoord loc, ChunkVoxelCoord voxLoc)
{
    RegionCoord regionLoc = chunkToRegion(loc);
    RegionChunkCoord chunkLoc = chunkToRegionChunk(loc);

    return mMods[regionLoc][chunkLoc][voxLoc];
}

void ModManager::deleteRegionFile(const RegionCoord& regionLoc)
{
    string indexFilename = getFilename(regionLoc) + indexExt;
    string dataFilename = getFilename(regionLoc) + dataExt;
    
    ifstream indexFile(indexFilename);
    if(indexFile)
    {
        indexFile.close();
        remove(indexFilename.c_str());
    }

    ifstream dataFile(dataFilename);
    if(dataFile)
    {
        dataFile.close();
        remove(dataFilename.c_str());
    }
}

ChunkIndex ModManager::getChunkIndex(RegionCoord regionLoc, RegionChunkCoord chunkLoc)
{
    hash<RegionChunkCoord> crcHash;
    ifstream indexFile(getFilename(regionLoc) + indexExt, ios::in | ios::binary);

    if(indexFile)
    {
        indexFile.seekg(crcHash(chunkLoc)*sizeof(ChunkIndex));

        ChunkIndex chunkIndex;
        indexFile.read((char*)&chunkIndex, sizeof(ChunkIndex));
        indexFile.close();

        return chunkIndex;
    } 
    else
    {
        initIndexFile(regionLoc);
        return NO_CHUNK;
    } 
}

void ModManager::initIndexFile(RegionCoord regionLoc)
{
    ofstream indexFile(getFilename(regionLoc) + indexExt, ios::out | ios::binary);

    ChunkIndex index = NO_CHUNK;
    for(int i = 0; i < regionWidth*regionWidth*regionWidth; ++i)
    {
        indexFile.write(reinterpret_cast<const char*>(&index), sizeof(ChunkIndex));
    }

    indexFile.close();
}

void ModManager::_setMod(const RegionCoord& regionLoc, const RegionChunkCoord& chunkLoc,const VoxelCoordinate_uint8& voxLoc, VoxelType type)
{
    if(voxLoc.x >= chunkWidth || voxLoc.y >= chunkWidth || voxLoc.z >= chunkWidth)
    {
        throw ModManagerException("Voxel location out of bounds.");
    }

    WorldModMap::const_iterator got = mMods.find(regionLoc);
    if(got == mMods.end())
    {
        mMods[regionLoc] = RegionModMap();
        mMods[regionLoc][chunkLoc] = ChunkModMap();
    }
    else
    {
        RegionModMap::const_iterator got = mMods[regionLoc].find(chunkLoc);
        if(got == mMods[regionLoc].end())
        {
            mMods[regionLoc][chunkLoc] = ChunkModMap();
        }
    }

    mMods[regionLoc][chunkLoc][voxLoc] = type;
}

std::string ModManager::getFilename(RegionCoord regionLoc)
{
    std::string xPart = std::to_string(regionLoc.x);
    std::string yPart = std::to_string(regionLoc.y);

    if(xPart[0] == '-')
        xPart[0] = '_';
    if(yPart[0] == '-')
        yPart[0] = '_';

    return regionDir + pathSep + xPart + "_" + yPart;  //NOTE: not sure why this needs to be casted... not good.
}
