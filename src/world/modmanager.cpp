#include <fea/assert.hpp>
#include "modmanager.hpp"
#include "../utilities/directorycreator.hpp"

using namespace std;

ModManager::ModManager(const std::string& worldPath) :
    mWorldPath(worldPath)
{
}

void ModManager::loadMods(const ChunkCoord& location, Chunk& chunk)
{
    ModRegionCoord modRegionLoc = ChunkToModRegion::convert(location);
    ModRegionChunkCoord chunkLoc = ChunkToModRegionChunk::convert(location); 

    ModRegionModMap::const_iterator got = mMods[modRegionLoc].find(chunkLoc);
    if(got == mMods[modRegionLoc].end())
    {
        ChunkIndex chunkIndex = getChunkIndex(modRegionLoc, chunkLoc);

        mMods[modRegionLoc][chunkLoc] = ChunkModMap();

        if(chunkIndex != NO_CHUNK) { 
            ifstream dataFile(getFilename(modRegionLoc) + modManagerExt, ios::in | ios::binary);
            dataFile.seekg(chunkIndex);

            uint16_t modCount;
            dataFile.read((char*)&modCount, sizeof(uint16_t));    

            uint64_t timestamp;
            dataFile.read((char*)&timestamp, sizeof(uint64_t));
            mTimestamps[modRegionLoc][chunkLoc] = timestamp;

            for(int i = 0; i < modCount; ++i) 
            {
                Mod mod;
                dataFile.read((char*)&mod, sizeof(Mod));
                _setMod(modRegionLoc, chunkLoc, mod.coord, mod.type);
            }
            dataFile.close();
        }
    }

    uint64_t timestamp;
    ModRegionTimestampMap::const_iterator got2 = mTimestamps[modRegionLoc].find(chunkLoc);
    if(got2 == mTimestamps[modRegionLoc].end())
    {
        timestamp = 0;
    } 
    else
    {
        timestamp = mTimestamps[modRegionLoc][chunkLoc];
    }

    ChunkModMap mods = mMods[modRegionLoc][chunkLoc];
    if(!mods.empty())
    { 
        VoxelTypeArray vta = chunk.getFlatVoxelTypeData();
        for(ChunkModMap::iterator it = mods.begin(); it != mods.end(); ++it) 
        {
            vta[it->first.x + it->first.z*chunkWidth + it->first.y*chunkWidthPow2] = it->second;
        }
        chunk.setVoxelData(vta);
    }
}

void ModManager::saveMods()
{
    for(WorldModMap::iterator it = mMods.begin(); it != mMods.end(); ++it)
    {
        saveMods(it->first);
    }
}

void ModManager::saveMods(ModRegionCoord modRegionLoc) 
{
    string dataFilename = getFilename(modRegionLoc) + modManagerExt;
    string indexFilename = getFilename(modRegionLoc) + modManagerIndexExt;

    hash<ModRegionChunkCoord> crcHash;
    ifstream iIndexFile(indexFilename, ios::in | ios::binary);

    if(iIndexFile)
    {
        ifstream iDataFile(dataFilename, ios::in | ios::binary);
        
        if(iDataFile)
        {
            for(int x = 0; x < modRegionWidthInChunks; ++x) 
            {
                for(int y = 0; y < modRegionWidthInChunks; ++y)
                {
                    for(int z = 0; z < modRegionWidthInChunks; ++z)
                    {
                        ModRegionChunkCoord chunkLoc(x, y, z);

                        ModRegionModMap::const_iterator got = mMods[modRegionLoc].find(chunkLoc);
                        if(got == mMods[modRegionLoc].end())
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
                                mTimestamps[modRegionLoc][chunkLoc] = timestamp;
                                mMods[modRegionLoc][chunkLoc] = ChunkModMap();
                                for(int i = 0; i < modCount; ++i)
                                {
                                    Mod mod;
                                    iDataFile.read((char*)&mod, sizeof(Mod));
                                    _setMod(modRegionLoc, chunkLoc, mod.coord, mod.type);
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
   
    initIndexFile(modRegionLoc);

    fstream oIndexFile(indexFilename, ios::binary | ios::in | ios::out);
    oIndexFile.clear();
    ofstream oDataFile(dataFilename, ios::out | ios::binary);

    ModRegionModMap modRegionMods = mMods[modRegionLoc];
    for(ModRegionModMap::iterator it = modRegionMods.begin(); it != modRegionMods.end(); ++it)
    {
        oIndexFile.seekp(crcHash(it->first)*sizeof(ChunkIndex));
        ChunkIndex index = oDataFile.tellp();
        oIndexFile.write(reinterpret_cast<const char*>(&index), sizeof(ChunkIndex));

        uint16_t modCount = it->second.size();
        oDataFile.write(reinterpret_cast<const char*>(&modCount), sizeof(modCount));

        ModRegionTimestampMap::const_iterator got = mTimestamps[modRegionLoc].find(it->first);
		
		FEA_ASSERT(got != mTimestamps[modRegionLoc].end(), "Chunk to be saved has not been timestamped");

        uint64_t timestamp = mTimestamps[modRegionLoc][it->first];
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

void ModManager::setMod(const VoxelCoord& voxLoc, VoxelType type)
{
    ModRegionCoord modRegionLoc = VoxelToModRegion::convert(voxLoc);
    ModRegionChunkCoord chunkLoc = ChunkToModRegionChunk::convert(VoxelToChunk::convert(voxLoc));

    _setMod(modRegionLoc, chunkLoc, VoxelToChunkVoxel::convert(voxLoc), type);
}

VoxelType ModManager::getMod(const VoxelCoord& voxLoc)
{
    ModRegionCoord modRegionLoc = VoxelToModRegion::convert(voxLoc);
    ModRegionChunkCoord chunkLoc = ChunkToModRegionChunk::convert(VoxelToChunk::convert(voxLoc));

    return mMods[modRegionLoc][chunkLoc][VoxelToChunkVoxel::convert(voxLoc)];
}

bool ModManager::hasMods(const ChunkCoord& location)
{
    bool result = false;

    ModRegionCoord modRegionLoc = ChunkToModRegion::convert(location);

    auto regionIterator = mMods.find(modRegionLoc);
    
    //first check if it is modified in memory
    if(regionIterator != mMods.end())
    {
        auto chunkIterator = regionIterator->second.find(ChunkToModRegionChunk::convert(location));

        if(chunkIterator != regionIterator->second.end())
        {
            result = chunkIterator->second.size() > 0;
        }
    }

    //if not modified in memory, check if it is modified in file
    if(!result)
    {
        string indexFilename = getFilename(modRegionLoc) + modManagerIndexExt;
        hash<ModRegionChunkCoord> crcHash;
        ifstream iIndexFile(indexFilename, ios::in | ios::binary);

        if(iIndexFile)
        {
            iIndexFile.seekg(crcHash(ChunkToModRegionChunk::convert(location))*sizeof(ChunkIndex));
            ChunkIndex chunkIndex;
            iIndexFile.read((char*)&chunkIndex, sizeof(ChunkIndex));
            result = chunkIndex != NO_CHUNK;
        }
    }

    return result;
}

void ModManager::setTimestamp(ChunkCoord loc, uint64_t timestamp)
{
    ModRegionCoord modRegionLoc = ChunkToModRegion::convert(loc);
    ModRegionChunkCoord chunkLoc = ChunkToModRegionChunk::convert(loc);

    mTimestamps[modRegionLoc][chunkLoc] = timestamp;
}

uint64_t ModManager::getTimestamp(ChunkCoord loc)
{
    ModRegionCoord modRegionLoc = ChunkToModRegion::convert(loc);
    ModRegionChunkCoord chunkLoc = ChunkToModRegionChunk::convert(loc);
	
	return mTimestamps[modRegionLoc][chunkLoc];
}

ChunkIndex ModManager::getChunkIndex(ModRegionCoord modRegionLoc, ModRegionChunkCoord chunkLoc)
{
    hash<ModRegionChunkCoord> crcHash;
    ifstream indexFile(getFilename(modRegionLoc) + modManagerIndexExt, ios::in | ios::binary);

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
        initIndexFile(modRegionLoc);
        return NO_CHUNK;
    } 
}

void ModManager::initIndexFile(ModRegionCoord modRegionLoc)
{
    ofstream indexFile(getFilename(modRegionLoc) + modManagerIndexExt, ios::out | ios::binary);

    ChunkIndex index = NO_CHUNK;
    for(int i = 0; i < modRegionWidthInChunks*modRegionWidthInChunks*modRegionWidthInChunks; ++i)
    {
        indexFile.write(reinterpret_cast<const char*>(&index), sizeof(ChunkIndex));
    }

    indexFile.close();
}

void ModManager::_setMod(const ModRegionCoord& modRegionLoc, const ModRegionChunkCoord& chunkLoc,const ChunkVoxelCoord& voxLoc, VoxelType type)
{
    FEA_ASSERT(!(voxLoc.x >= chunkWidth || voxLoc.y >= chunkWidth || voxLoc.z >= chunkWidth), "internal coordinates out of bounds. Something is wrong with coordinate conversion");

    mMods[modRegionLoc][chunkLoc][voxLoc] = type;
}

std::string ModManager::getFilename(ModRegionCoord modRegionLoc)
{
    std::string xPart = std::to_string(modRegionLoc.x);
    std::string yPart = std::to_string(modRegionLoc.y);
    std::string zPart = std::to_string(modRegionLoc.z);

    if(xPart[0] == '-')
        xPart[0] = '_';
    if(yPart[0] == '-')
        yPart[0] = '_';
    if(zPart[0] == '-')
        zPart[0] = '_';

    return mWorldPath + "/" + xPart + "_" + yPart + "_" + zPart; 
}
