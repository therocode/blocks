#include "worldentry.hpp"
#include "worldmessages.hpp"
#include "../utilities/interpolators.hpp"
#include <algorithm>

WorldEntry::WorldEntry(fea::MessageBus& bus, const std::unordered_map<BiomeId, Biome>& biomes,  WorldId id, const std::string& identifier, const WorldData& data, const std::string& path) :
    mBus(bus),
    mBiomes(biomes),
    mId(id),
    mIdentifier(identifier),
    mWorldData(data),
    mModManager(path),
	mExplorationManager(path)
{
    mBus.send(WorldBiomeSettingsMessage{id, mWorldData.biomeSettings});

	fea::subscribe(mBus, *this);
}

WorldEntry::WorldEntry(WorldEntry&& other) :
        mBus(other.mBus),
        mBiomes(other.mBiomes),
        mId(other.mId),
        mIdentifier(other.mIdentifier),
        mWorldData(std::move(other.mWorldData)),
        mBiomeGridNotifier(std::move(other.mBiomeGridNotifier)),
        mHighlightManager(std::move(other.mHighlightManager)),
        mModManager(std::move(other.mModManager)),
		mExplorationManager(std::move(other.mExplorationManager)),
		mCurrentFrameNumber(other.mCurrentFrameNumber),
        mPendingChunksToRequests(std::move(other.mPendingChunksToRequests))
{
	fea::subscribe(mBus, *this);
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

    mModManager.saveMods();  //this is a hack due to issue http://dev.pallkars.net/issues/210
}

void WorldEntry::deliverBiome(const BiomeRegionCoord& coordinate, const BiomeGrid& biomeData, const FieldMap& fields)
{
    if(mBiomeGridNotifier.isActive(coordinate))
    {
        mWorldData.fieldGrids.emplace(coordinate, std::move(fields));
        mWorldData.biomeGrids.emplace(coordinate, biomeData);

        auto iterator = mPendingChunksToRequests.find(coordinate);

        if(iterator != mPendingChunksToRequests.end())
        {
            for(const auto& chunk : iterator->second)
            {
                requestChunk(chunk);
            }

            mPendingChunksToRequests.erase(iterator);
        }
    }
}

void WorldEntry::deliverChunk(const ChunkCoord& coordinate, const Chunk& chunk)
{
    if(mHighlightManager.chunkIsHighlighted(coordinate))
    {
        Chunk& createdChunk = mWorldData.voxels.emplace(coordinate, chunk).first->second;

		uint16_t frameNumberDelta = mCurrentFrameNumber;
		if(!mExplorationManager.getChunkExplored(coordinate)) //explore the chunk and perform initial generation if not already explored
		{
            VoxelTypeArray before = createdChunk.getFlatVoxelTypeData();

			mBus.send(ChunkInitiallyGeneratedMessage{mId, coordinate, createdChunk});
			mExplorationManager.setChunkExplored(coordinate);
            
            applyDifferenceAsMods(coordinate, before, createdChunk.getFlatVoxelTypeData());
			
			mModManager.setTimestamp(coordinate, mCurrentFrameNumber);
		}
        else       //apply possible modifications on chunks that are already explored
        {
			frameNumberDelta = mCurrentFrameNumber - mModManager.getTimestamp(coordinate);
            mModManager.loadMods(coordinate, createdChunk);
			mModManager.setTimestamp(coordinate, mCurrentFrameNumber);
        }

		mExplorationManager.activateChunk(coordinate);
		
        VoxelTypeArray before = createdChunk.getFlatVoxelTypeData();

        mBus.send(ChunkCandidateMessage{mId, coordinate, createdChunk, frameNumberDelta}); //let others have a chance of modifying this chunk

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

void WorldEntry::handleMessage(const FrameMessage& received)
{
	mCurrentFrameNumber = received.frameNumber;
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
        {
            iterator->second.erase(std::remove(iterator->second.begin(), iterator->second.end(), chunkCoordinate),
                                           iterator->second.end());
        }
    }
}

void WorldEntry::requestChunk(const ChunkCoord& chunk)
{
    BiomeGrid grid(chunkWidth + biomeInterpolationWidth, biomeDownSamplingAmount);
    grid.setInterpolator(Interpolator<BiomeId>::nearestNeigbor);
    BiomeRegionChunkCoord biomeRegionChunk = ChunkToBiomeRegionChunk::convert(chunk);
    BiomeGrid& bigGrid = mWorldData.biomeGrids.at(ChunkToBiomeRegion::convert(chunk));

    FieldMap fields;
    const FieldMap& bigFieldMap = mWorldData.fieldGrids.at(ChunkToBiomeRegion::convert(chunk));

    uint32_t size = grid.getInnerSize();
    uint32_t step = biomeInterpolationWidth;
    glm::uvec3 start = (glm::uvec3)biomeRegionChunk * (uint32_t)chunkWidth;

    for(const auto& bigField : bigFieldMap)
    {
        FieldGrid newFieldGrid(chunkWidth + biomeInterpolationWidth, biomeDownSamplingAmount);
        newFieldGrid.setInterpolator(Interpolator<float>::nearestNeigbor);
        fields.emplace(bigField.first, newFieldGrid);
    }

    glm::uvec3 coordinate;
    glm::uvec3 innerCoordinate;

    for(uint32_t z = 0; z < size; z++)
    {
        for(uint32_t y = 0; y < size; y++)
        {
            for(uint32_t x = 0; x < size; x++)
            {
                coordinate = start + glm::uvec3(x, y, z) * step; //can trivially be optimised by using addition in the loops if necessary

                grid.setInner({x, y, z}, bigGrid.get(coordinate));

                for(const auto& bigField : bigFieldMap)
                {
                    fields.at(bigField.first).setInner({x, y, z}, bigField.second.get(coordinate));
                }
            }
        }
    }

    mBus.send(ChunkGenerationRequestedMessage{0, mId, chunk, grid, fields});
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
