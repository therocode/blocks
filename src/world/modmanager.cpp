#include "modmanager.hpp"
#include "../utilities/directorycreator.hpp"

using namespace std;

ModManager::ModManager(const std::string& worldName) :
    mWorldName(worldName)
{
}

void ModManager::loadMods(const ChunkCoord& location, Chunk& chunk)
{
    RegionCoord regionLoc = ChunkToRegion::convert(location);
    RegionChunkCoord chunkLoc = ChunkToRegionChunk::convert(location); 

    RegionModMap::const_iterator got = mMods[regionLoc].find(chunkLoc);
    if(got == mMods[regionLoc].end())
    {
        ChunkIndex chunkIndex = getChunkIndex(regionLoc, chunkLoc);

        mMods[regionLoc][chunkLoc] = ChunkModMap();

        if(chunkIndex != NO_CHUNK) { 
            ifstream dataFile(getFilename(regionLoc) + dataExt, ios::in | ios::binary);
            dataFile.seekg(chunkIndex);

            uint16_t modCount;
            dataFile.read((char*)&modCount, sizeof(uint16_t));    

            uint64_t timestamp;
            dataFile.read((char*)&timestamp, sizeof(uint64_t));
            mTimestamps[regionLoc][chunkLoc] = timestamp;

            for(int i = 0; i < modCount; ++i) 
            {
                Mod mod;
                dataFile.read((char*)&mod, sizeof(Mod));
                _setMod(regionLoc, chunkLoc, mod.coord, mod.type);
            }
            dataFile.close();
        }
    }

    uint64_t timestamp;
    RegionTimestampMap::const_iterator got2 = mTimestamps[regionLoc].find(chunkLoc);
    if(got2 == mTimestamps[regionLoc].end())
    {
        timestamp = 0;
    } 
    else
    {
        timestamp = mTimestamps[regionLoc][chunkLoc];
    }

    ChunkModMap mods = mMods[regionLoc][chunkLoc];
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

void ModManager::saveMods(RegionCoord regionLoc) 
{
	if(!DirectoryCreator::directoryExists(regionDir))
	{
		if(!DirectoryCreator::createDirectory(regionDir))
		{
			std::cout << regionDir << "didn't exist and failed to create it" << std::endl;
		}
	}
	if(!DirectoryCreator::directoryExists(regionDir + pathSep + mWorldName))
	{
		if(!DirectoryCreator::createDirectory(regionDir + pathSep + mWorldName))
		{
			std::cout << regionDir + pathSep + mWorldName << "didn't exist and failed to create it" << std::endl;
		}
	}

    string dataFilename = getFilename(regionLoc) + dataExt;
    string indexFilename = getFilename(regionLoc) + indexExt;

    hash<RegionChunkCoord> crcHash;
    ifstream iIndexFile(indexFilename, ios::in | ios::binary);

    if(iIndexFile)
    {
        ifstream iDataFile(dataFilename, ios::in | ios::binary);
        
        if(iDataFile)
        {
            for(int x = 0; x < regionChunkWidth; ++x) 
            {
                for(int y = 0; y < regionChunkWidth; ++y)
                {
                    for(int z = 0; z < regionChunkWidth; ++z)
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
                                mTimestamps[regionLoc][chunkLoc] = timestamp;
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

        RegionTimestampMap::const_iterator got = mTimestamps[regionLoc].find(it->first);
        if(got == mTimestamps[regionLoc].end())
        {
            //throw ModManagerException("Chunk to be saved has not been timestamped");
            mTimestamps[regionLoc][it->first] = 0;
        }

        uint64_t timestamp = mTimestamps[regionLoc][it->first];
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

void ModManager::setMod(ChunkCoord loc, ChunkVoxelCoord voxLoc, VoxelType type)
{
    RegionCoord regionLoc = ChunkToRegion::convert(loc);
    RegionChunkCoord chunkLoc = ChunkToRegionChunk::convert(loc);

    _setMod(regionLoc, chunkLoc, voxLoc, type);
}

VoxelType ModManager::getMod(ChunkCoord loc, ChunkVoxelCoord voxLoc)
{
    RegionCoord regionLoc = ChunkToRegion::convert(loc);
    RegionChunkCoord chunkLoc = ChunkToRegionChunk::convert(loc);

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

void ModManager::recordTimestamp(ChunkCoord loc, uint64_t timestamp)
{
    RegionCoord regionLoc = ChunkToRegion::convert(loc);
    RegionChunkCoord chunkLoc = ChunkToRegionChunk::convert(loc);

    mTimestamps[regionLoc][chunkLoc] = timestamp;
}

void ModManager::setWorldName(const std::string& name)
{
    mWorldName = name;
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
    for(int i = 0; i < regionChunkWidth*regionChunkWidth*regionChunkWidth; ++i)
    {
        indexFile.write(reinterpret_cast<const char*>(&index), sizeof(ChunkIndex));
    }

    indexFile.close();
}

void ModManager::_setMod(const RegionCoord& regionLoc, const RegionChunkCoord& chunkLoc,const ChunkVoxelCoord& voxLoc, VoxelType type)
{
    if(voxLoc.x >= chunkWidth || voxLoc.y >= chunkWidth || voxLoc.z >= chunkWidth)
    {
        throw ModManagerException("Voxel location out of bounds.");
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

    return regionDir + pathSep + mWorldName + pathSep + xPart + "_" + yPart;  //NOTE: not sure why this needs to be casted... not good.
}
