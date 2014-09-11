#include "worldprovider.h"
#include "region.h"
#include "biome.h"
#include "../application/applicationmessages.h"
#include <iostream>

WorldProvider::WorldProvider(fea::MessageBus& b, RegionStorageInterface& mRegionGenerator, ModManager& modManager)
    :
    mBus(b),
    mRegionStorage(mRegionGenerator),
    mModManager(modManager)
{
    mBus.addSubscriber<ChunkRequestedMessage>(*this);
}

WorldProvider::~WorldProvider()
{
    mBus.removeSubscriber<ChunkRequestedMessage>(*this);
}

void WorldProvider::handleMessage(const ChunkRequestedMessage& received)
{
    //NOTE: I need to add the chunk request to the thread








    ChunkCoord chunkCoordinate = received.coordinate;

    RegionCoord regionCoordinate = chunkToRegion(chunkCoordinate);

    if(!mRegionStorage.hasRegion(regionCoordinate))
    {
        Region newRegion = mRegionGenerator.generateRegion(regionCoordinate);
        mBus.send(RegionDeliverMessage{regionCoordinate, newRegion});
    }

    const Region& region = mRegionStorage.getRegion(regionCoordinate);

    Chunk newChunk = generateChunk(chunkCoordinate, region);

    //mBus.send(LogMessage("loading modifications for chunk" + glm::to_string((glm::ivec3)chunkCoordinate), "file", LogLevel::VERB));
    mModManager.loadMods(newChunk);
    uint64_t timestamp = 0; //get proper timestamp later

    mBus.send(ChunkDeliverMessage{chunkCoordinate, newChunk}); //sends the finished chunk to be kept by whatever system

    mBus.send(ChunkLoadedMessage{chunkCoordinate, timestamp}); //the now fully initialised chunk is announced to the rest of the game. should it be here?
}

Chunk WorldProvider::generateChunk(const ChunkCoord& chunkCoordinate, const Region& region) const
{
    Chunk newChunk(chunkCoordinate);

    VoxelCoord chunkVoxelCoord = worldToVoxel(chunkToWorld(chunkCoordinate));
    //std::cout << chunkVoxelCoord.z << " is z \n";
    RegionVoxelCoord regionStart = voxelToRegionVoxel(chunkVoxelCoord);
    RegionVoxelCoord regionCoord = regionStart;

    float chunkY = chunkVoxelCoord.y;

    VoxelTypeArray voxelData;

    size_t yIndex = 0;
    size_t zIndex = 0;

    for(int32_t y = 0; y < chunkWidth; y++)
    {
        yIndex = y * chunkWidthx2;
        regionCoord.y = regionStart.y;
        for(int32_t z = 0; z < chunkWidth; z++)
        {
            zIndex = z * chunkWidth;
            regionCoord.x = regionStart.x;
            for(int32_t x = 0; x < chunkWidth; x++)
            {
                //std::cout << "voxel coord for chunk is " << voxelCoord.x << " " << voxelCoord.y << " " << voxelCoord.z << "\n";
                //if(chunkCoordinate == ChunkCoord(1,0,3))
                //std::cout << "region coord: " << regionCoord.x << " " << regionCoord.y << "\n";
                float worldY = (float)(chunkY + y);
                float threshold = region.getHeightmap().getUnit(regionCoord.x, regionCoord.y) * 100.f;
                const Biome* biome = region.getBiome(regionCoord);
                VoxelType biomeType = biome->mType;

                voxelData[zIndex + yIndex + x] = (worldY < threshold) ? biomeType : 0;

                regionCoord.x++;
            }
            regionCoord.y++;
        }
    }

    newChunk.setVoxelData(voxelData);

    return newChunk;
}
