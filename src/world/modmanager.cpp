#include <fea/assert.hpp>
#include "modmanager.hpp"
#include "../utilities/directorycreator.hpp"

using namespace std;

ModManager::ModManager(const std::string& worldName) :
    mWorldName(worldName)
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
            ifstream dataFile(getFilename(modRegionLoc) + dataExt, ios::in | ios::binary);
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
	if(!DirectoryCreator::directoryExists(modRegionDir))
	{
		if(!DirectoryCreator::createDirectory(modRegionDir))
		{
			std::cout << modRegionDir << "didn't exist and failed to create it" << std::endl;
		}
	}
	if(!DirectoryCreator::directoryExists(modRegionDir + pathSep + mWorldName))
	{
		if(!DirectoryCreator::createDirectory(modRegionDir + pathSep + mWorldName))
		{
			std::cout << modRegionDir + pathSep + mWorldName << "didn't exist and failed to create it" << std::endl;
		}
	}

    string dataFilename = getFilename(modRegionLoc) + dataExt;
    string indexFilename = getFilename(modRegionLoc) + indexExt;

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
        if(got == mTimestamps[modRegionLoc].end())
        {
            //throw ModManagerException("Chunk to be saved has not been timestamped");
            mTimestamps[modRegionLoc][it->first] = 0;
        }

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

void ModManager::deleteModRegionFile(const ModRegionCoord& modRegionLoc)
{
    string indexFilename = getFilename(modRegionLoc) + indexExt;
    string dataFilename = getFilename(modRegionLoc) + dataExt;
    
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

void ModManager::recordTimestamp(ChunkCoord loc, uint64_t timestamp)
{
    ModRegionCoord modRegionLoc = ChunkToModRegion::convert(loc);
    ModRegionChunkCoord chunkLoc = ChunkToModRegionChunk::convert(loc);

    mTimestamps[modRegionLoc][chunkLoc] = timestamp;
}

void ModManager::setWorldName(const std::string& name)
{
    mWorldName = name;
}

ChunkIndex ModManager::getChunkIndex(ModRegionCoord modRegionLoc, ModRegionChunkCoord chunkLoc)
{
    hash<ModRegionChunkCoord> crcHash;
    ifstream indexFile(getFilename(modRegionLoc) + indexExt, ios::in | ios::binary);

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
    ofstream indexFile(getFilename(modRegionLoc) + indexExt, ios::out | ios::binary);

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

    if(xPart[0] == '-')
        xPart[0] = '_';
    if(yPart[0] == '-')
        yPart[0] = '_';

    return modRegionDir + pathSep + mWorldName + pathSep + xPart + "_" + yPart;  //NOTE: not sure why this needs to be casted... not good.
}
