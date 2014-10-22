#include "worldentry.hpp"
#include "worldmessages.hpp"
#include "../utilities/interpolators.hpp"
#include <algorithm>

WorldEntry::WorldEntry(fea::MessageBus& bus, WorldId id, const std::string& identifier, const WorldData& data, const std::string& path) :
    mBus(bus),
    mId(id),
    mIdentifier(identifier),
    mWorldData(data),
    mModManager(path)
{

    mWorldData.biomeSettings.fields =
    {
        Field{"height"},
        Field{"rainfall"},
        Field{"temperature"},
        Field{"selector"}
    };

    mBus.send(WorldBiomeSettingsMessage{id, mWorldData.biomeSettings});
}

WorldEntry::~WorldEntry()
{
    mModManager.saveMods();
}

void WorldEntry::addHighlightEntity(uint32_t id, const ChunkCoord& location, uint32_t radius)
{
    const auto& highlighted = mHighlightManager.addHighlightEntity(id, location, radius);

    for(const auto& chunk : highlighted)
        activateChunk(chunk);
}

void WorldEntry::moveHighlightEntity(uint32_t id, const ChunkCoord& location)
{
    const auto& highlightedDehighlighted = mHighlightManager.moveHighlightEntity(id, location);

    for(const auto& chunk : highlightedDehighlighted.first)
        activateChunk(chunk);

    for(const auto& chunk : highlightedDehighlighted.second)
        deactivateChunk(chunk);
}


void WorldEntry::removeHighlightEntity(uint32_t id)
{
    const auto& dehighlighted = mHighlightManager.removeHighlightEntity(id);

    for(const auto& chunk : dehighlighted)
        deactivateChunk(chunk);

    mModManager.saveMods();  //this is a hack due to issue http://dev.pallkars.net/issues/21
}

void WorldEntry::deliverBiome(const BiomeRegionCoord& coordinate, const BiomeGrid& biomeData)
{
    if(mBiomeGridNotifier.isActive(coordinate))
    {
        mWorldData.biomeGrids.emplace(coordinate, biomeData);

        auto iterator = mPendingChunksToRequests.find(coordinate);

        if(iterator != mPendingChunksToRequests.end())
        {
            for(const auto& chunk : iterator->second)
            {
                requestChunk(chunk);
            }

            mPendingChunksToRequests.erase(coordinate);
        }
    }
}

void WorldEntry::deliverChunk(const ChunkCoord& coordinate, const Chunk& chunk)
{
    if(mHighlightManager.chunkIsHighlighted(coordinate))
    {
        auto iterator = mWorldData.voxels.emplace(coordinate, chunk);

        //#C1#if(mModManager.hasMods(chunk))  // implement this check as seen in issue #143
        //mModManager.loadMods(chunk);
        //#C2#else      // implement mExplorationManager as according to issue #144
        //#C2#{
        //#C2#    if(!mExplorationManager.isExplored(regionCoord))
        //#C2#        mBus.send(ChunkInitiallyGeneratedMessage{coordinate, chunk})
        //#C2#}

        uint64_t timestamp = 0; //#C3# get proper timestamp, issue #133

        mModManager.loadMods(coordinate, iterator.first->second);//temporary

        mBus.send(ChunkCandidateMessage{mId, coordinate, iterator.first->second, timestamp});

        mBus.send(ChunkFinishedMessage{mId, coordinate, iterator.first->second}); //the now fully initialised chunk is announced to the rest of the game.
    }
}

const ChunkMap& WorldEntry::getChunkMap() const
{
    return mWorldData.voxels;
}

ChunkMap& WorldEntry::getChunkMap()
{
    return mWorldData.voxels;
}

void WorldEntry::setVoxelType(const VoxelCoord& voxelCoordinate, VoxelType type)
{
    const ChunkCoord& chunkCoord = VoxelToChunk::convert(voxelCoordinate);
    const ChunkVoxelCoord& chunkVoxelCoord = VoxelToChunkVoxel::convert(voxelCoordinate);

    auto chunk = mWorldData.voxels.find(chunkCoord);

    if(chunk != mWorldData.voxels.end())
    {
        chunk->second.setVoxelType(chunkVoxelCoord, type);
        mBus.send(VoxelSetMessage{mId, voxelCoordinate, type});

        mModManager.setMod(voxelCoordinate, type);
    }
}

void WorldEntry::chunksRequested(const std::vector<ChunkCoord>& coordinates)
{
    std::unordered_map<ChunkCoord, const Chunk&> grantedChunks;
    std::vector<ChunkCoord> deniedChunks;

    for(const auto& requestedChunk : coordinates)
    {
        if(mHighlightManager.chunkIsHighlighted(requestedChunk))
        {
            const auto& iterator = mWorldData.voxels.find(requestedChunk);

            if(iterator != mWorldData.voxels.end())
            {
                grantedChunks.emplace(requestedChunk, iterator->second);
            }
        }
        else
        {
            deniedChunks.push_back(requestedChunk);
        }
    }

    if(grantedChunks.size() > 0)
    {
        ChunksDataDeliveredMessage message{mId, grantedChunks};

        mBus.send(message);
    }
    if(deniedChunks.size() > 0)
    {
        ChunksDataDeniedMessage message{mId, deniedChunks};

        mBus.send(message);
    }
}

const std::string& WorldEntry::getIdentifier() const
{
    return mIdentifier;
}

void WorldEntry::activateChunk(const ChunkCoord& chunkCoordinate)
{
    if(mWorldData.range.isWithin(chunkCoordinate))
    {
        //firsty handle any newly activated biomes; they need to be requested
        const auto& activatedBiomes = mBiomeGridNotifier.set(chunkCoordinate, true);

        for(const auto& biomeRegionCoord : activatedBiomes)
        {
            mBus.send(BiomeRequestedMessage{mId, biomeRegionCoord});
        }

        //secondly, the activated chunk will need to be generated as well, but we can't do that if the biome hasn't been delivered since we need data from the biome to pass on to the generator. Hence, if the biome doesn't exist, we store the request
        const BiomeRegionCoord biomeRegionCoord = ChunkToBiomeRegion::convert(chunkCoordinate);
        auto iterator = mWorldData.biomeGrids.find(biomeRegionCoord);
        if(iterator != mWorldData.biomeGrids.end())
        {
            requestChunk(chunkCoordinate);
        }
        else
        {
            mPendingChunksToRequests[biomeRegionCoord].push_back(chunkCoordinate);
        }
    }
}

void WorldEntry::deactivateChunk(const ChunkCoord& chunkCoordinate)
{
    if(mWorldData.range.isWithin(chunkCoordinate))
    {
        //first lets see if this also deactivates a biome. in that case, it must be removed
        const auto& deactivatedBiomes = mBiomeGridNotifier.set(chunkCoordinate, false);

        for(const auto& biomeRegionCoord : deactivatedBiomes)
        {
            mWorldData.biomeGrids.erase(biomeRegionCoord);
            mPendingChunksToRequests.erase(biomeRegionCoord); //if the biome is deactivated, we don't need to send pending requests either.
        }

        if(mWorldData.voxels.erase(chunkCoordinate) != 0)
        {
            //mBus.send(ChunkDeletedMessage{chunkCoordinate}); there is no such message atm
        }

        //we don't need to send pending requests for this chunk
        const BiomeRegionCoord& biomeRegionCoord = ChunkToBiomeRegion::convert(chunkCoordinate);
        auto iterator = mPendingChunksToRequests.find(biomeRegionCoord);

        if(iterator != mPendingChunksToRequests.end())
            std::remove(iterator->second.begin(), iterator->second.end(), chunkCoordinate);
    }
}

void WorldEntry::requestChunk(const ChunkCoord& chunk)
{
    BiomeGrid grid(16, 4);
    grid.setInterpolator(Interpolator<BiomeIndex>::nearestNeigbor);
    BiomeRegionChunkCoord biomeRegionChunk = ChunkToBiomeRegionChunk::convert(chunk);
    BiomeGrid& bigGrid = mWorldData.biomeGrids.at(ChunkToBiomeRegion::convert(chunk));

    uint32_t size = grid.getInnerSize();
    glm::uvec3 start = (glm::uvec3)biomeRegionChunk / biomeRegionWidthInChunks;

    for(uint32_t z = 0; z < size; z++)
    {
        for(uint32_t y = 0; y < size; y++)
        {
            for(uint32_t x = 0; x < size; x++)
            {
                grid.setInner({x, y, z}, bigGrid.get(start + glm::uvec3(x, y, z)));
            }
        }
    }

    mBus.send(ChunkGenerationRequestedMessage{0, mId, chunk, grid});
}
