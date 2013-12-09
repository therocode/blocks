#include "region.h"
#include "worldmessages.h"
#include <iostream>

Region::Region(fea::MessageBus& bus) : mBus(bus)
{
}

void Region::setChunkProvider(std::unique_ptr<ChunkProvider> del)
{
    chunkDeliverer = std::move(del);
}

Chunk& Region::loadChunk(const ChunkCoordinate& location)
{
    auto chunk= chunks.find(location);
    if(chunk == chunks.end())
    {
        chunks.emplace(location, chunkDeliverer->fetchChunk(location));
        mBus.sendMessage<ChunkCreatedMessage>(ChunkCreatedMessage(location, chunks.at(location).getVoxelTypeData().mRleSegmentIndices, chunks.at(location).getVoxelTypeData().mRleSegments));
    }

    return chunks.at(location);
}

bool Region::chunkIsLoaded(const ChunkCoordinate& location) const
{
    return chunks.find(location) != chunks.end();
}

const Chunk& Region::getChunk(const ChunkCoordinate& location) const
{
    return chunks.at(location);
}

Chunk& Region::getChunk(const ChunkCoordinate& location)
{
    return chunks.at(location);
}

ChunkReferenceMap Region::getChunkList() const
{
    return ChunkReferenceMap();
}

void Region::highlightChunk(size_t id, const ChunkCoordinate& chunk)
{
    int32_t halfCheatBoxWidth = 6;

    int32_t centerX = chunk.x;
    int32_t centerY = chunk.y;
    int32_t centerZ = chunk.z;

    for(int32_t x = centerX - halfCheatBoxWidth; x <= centerX + halfCheatBoxWidth; x++)
    {
        for(int32_t y = centerY - halfCheatBoxWidth / 3; y <= centerY + halfCheatBoxWidth / 3; y++)
        {
            for(int32_t z = centerZ - halfCheatBoxWidth; z <= centerZ + halfCheatBoxWidth; z++)
            {
                ChunkCoordinate coordinate(x, y, z);
                loadChunk(coordinate);
            }
        }
    }

    highlightedChunks[id] = chunk;
    checkUnloads(id);
}

void Region::checkUnloads(size_t id)
{
    if(highlightedChunks.find(id) != highlightedChunks.end())
    {
        std::vector<ChunkCoordinate> chunksToUnload;

        ChunkCoordinate& highlighted = highlightedChunks.at(id);
        for(auto& chunk : chunks)
        {
            ChunkCoordinate distance = highlighted - chunk.first;

            if(abs(distance.x + distance.z) > 12 || abs(distance.y) > 6)
            {
                chunksToUnload.push_back(chunk.first);
            }
        }

        for(auto& chunk : chunksToUnload)
        {
            //unloadChunk(chunk);
        }
    }
}

void Region::unloadChunk(const ChunkCoordinate& chunk)
{
    mBus.sendMessage<ChunkDeletedMessage>(ChunkDeletedMessage(chunk));
    chunks.erase(chunk);
}
