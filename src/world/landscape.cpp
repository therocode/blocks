#include "landscape.h"
#include "worldmessages.h"
#include <iostream>

Landscape::Landscape(fea::MessageBus& bus) : mBus(bus)
{
}

void Landscape::setChunkProvider(std::unique_ptr<ChunkProvider> del)
{
    chunkDeliverer = std::move(del);
}

Chunk& Landscape::loadChunk(const ChunkCoordinate& location)
{
    auto chunk= chunks.find(location);
    if(chunk == chunks.end())
    {
        chunks.emplace(location, chunkDeliverer->fetchChunk(location));
        mBus.sendMessage<ChunkCreatedMessage>(ChunkCreatedMessage(location, chunks.at(location).getVoxelTypeData().mRleSegmentIndices, chunks.at(location).getVoxelTypeData().mRleSegments));
    }

    return chunks.at(location);
}

bool Landscape::chunkIsLoaded(const ChunkCoordinate& location) const
{
    return chunks.find(location) != chunks.end();
}

const Chunk& Landscape::getChunk(const ChunkCoordinate& location) const
{
    return chunks.at(location);
}

Chunk& Landscape::getChunk(const ChunkCoordinate& location)
{
    return chunks.at(location);
}

const ChunkMap& Landscape::getChunkList() const
{
    return chunks;
}

void Landscape::highlightChunk(size_t id, const ChunkCoordinate& chunk)
{

    int32_t halfCheatBoxWidth = 8;

    int32_t centerX = chunk.x;
    int32_t centerY = chunk.y;
    int32_t centerZ = chunk.z;
    
    for(int32_t x = centerX - halfCheatBoxWidth; x <= centerX + halfCheatBoxWidth; x++)
    {
        for(int32_t y = centerY - halfCheatBoxWidth / 4; y <= centerY + halfCheatBoxWidth / 4; y++)
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

void Landscape::checkUnloads(size_t id)
{
    if(highlightedChunks.find(id) != highlightedChunks.end())
    {
        std::vector<ChunkCoordinate> chunksToUnload;

        ChunkCoordinate& highlighted = highlightedChunks.at(id);
        for(auto& chunk : chunks)
        {
            ChunkCoordinate distance = highlighted - chunk.first;

            if(abs(distance.x + distance.z) > 6 || abs(distance.y) > 3)
            {
                chunksToUnload.push_back(chunk.first);
            }
        }

        for(auto& chunk : chunksToUnload)
        {
            unloadChunk(chunk);
        }
    }
}

void Landscape::unloadChunk(const ChunkCoordinate& chunk)
{
    mBus.sendMessage<ChunkDeletedMessage>(ChunkDeletedMessage(chunk));
    chunks.erase(chunk);
}
