#include "worldentry.hpp"
#include "worldmessages.hpp"
#include "../utilities/interpolators.hpp"
#include <algorithm>

WorldEntry::WorldEntry(fea::MessageBus& bus, WorldId id, const std::string& identifier, const WorldData& data, const std::string& path) :
    mBus(bus),
    mId(id),
    mIdentifier(identifier),
    mWorldData(data),
    mModManager(path),
	mExplorationManager(path)
{
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

void WorldEntry::deliverBiome(const BiomeRegionCoord& coordinate, const FieldMap& fields)
{
    if(mBiomeGridNotifier.isActive(coordinate))
    {
        mWorldData.biomeGrids.emplace(coordinate, generateBiomes(fields));
        mWorldData.fieldGrids.emplace(coordinate, std::move(fields));

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
        Chunk& createdChunk = mWorldData.voxels.emplace(coordinate, chunk).first->second;

		if(!mExplorationManager.getChunkExplored(coordinate)) //explore the chunk and perform initial generation if not already explored
		{
            VoxelTypeArray before = createdChunk.getFlatVoxelTypeData();

			mBus.send(ChunkInitiallyGeneratedMessage{mId, coordinate, createdChunk});
			mExplorationManager.setChunkExplored(coordinate);
            
            applyDifferenceAsMods(coordinate, before, createdChunk.getFlatVoxelTypeData());
		}
        else       //apply possible modifications on chunks that are already explored
        {
            mModManager.loadMods(coordinate, createdChunk);
        }

		mExplorationManager.activateChunk(coordinate);
		
        uint64_t timestamp = 0; //#C3# get proper timestamp, issue #133

        VoxelTypeArray before = createdChunk.getFlatVoxelTypeData();

        mBus.send(ChunkCandidateMessage{mId, coordinate, createdChunk, timestamp}); //let others have a chance of modifying this chunk

        applyDifferenceAsMods(coordinate, before, createdChunk.getFlatVoxelTypeData());

        mBus.send(ChunkFinishedMessage{mId, coordinate, createdChunk}); //the now fully initialised chunk is announced to the rest of the game.
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
        //firstly handle any newly activated biomes; they need to be requested
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
            mWorldData.fieldGrids.erase(biomeRegionCoord);
            mPendingChunksToRequests.erase(biomeRegionCoord); //if the biome is deactivated, we don't need to send pending requests either.
        }

        if(mWorldData.voxels.erase(chunkCoordinate) != 0)
        {
			mExplorationManager.deactivateChunk(chunkCoordinate);
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
    grid.setInterpolator(Interpolator<BiomeId>::nearestNeigbor);
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

void WorldEntry::applyDifferenceAsMods(const ChunkCoord& coordinate, const VoxelTypeArray& before, const VoxelTypeArray& after)
{
    uint32_t index = 0;

    for(uint32_t y = 0; y < chunkWidth; y++)
    {
        for(uint32_t z = 0; z < chunkWidth; z++)
        {
            for(uint32_t x = 0; x < chunkWidth; x++)
            {
                if(before[index] != after[index])
                {
                    mModManager.setMod(ChunkToVoxel::convert(coordinate) + VoxelCoord(x, y, z), after[index]);
                }
                index++;
            }
        }
    }
}

BiomeGrid WorldEntry::generateBiomes(const FieldMap& fields) const
{
    BiomeGrid result(biomeRegionWidth, 4);
    //for(uint32_t z = 0; z < size; z++)
    //{
    //    for(uint32_t y = 0; y < size; y++)
    //    {
    //        for(uint32_t x = 0; x < size; x++)
    //        {
    //            approvedPointBiomes.clear();

    //            for(uint32_t i = 0; i < worldBiomes.size(); i++)
    //            {
    //                const Biome& biome = *worldBiomes[i];

    //                bool include = true;

    //                for(const auto& field : fieldGrids)
    //                {
    //                    if(field.first == "selector")
    //                    {
    //                        continue;
    //                    }
    //                    else if(biome.mRequirements.count(field.first) == 0)
    //                    {
    //                        continue;
    //                    }

    //                    if(!biome.mRequirements.at(field.first).isWithin(field.second.getInner({x, y, z})))
    //                    {
    //                        include = false;
    //                        break;
    //                    }
    //                }

    //                if(include)
    //                {
    //                    approvedPointBiomes.push_back(&biome);
    //                }
    //            };

    //            float selectPercent = fieldGrids.at("selector").getInner({x, y, z});
    //            BiomeIndex selectedBiome = mBiomeNameToIndex.at(approvedPointBiomes[(uint32_t)((float)approvedPointBiomes.size() * selectPercent)]->mName);

    //            biomeData.setInner({x, y, z}, selectedBiome); 
    //        }
    //    }
    //}

    result.setInterpolator(Interpolator<BiomeId>::nearestNeigbor);
    return result;
}
