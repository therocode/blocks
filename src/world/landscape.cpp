#include "landscape.h"
#include "worldmessages.h"

Landscape::Landscape(fea::MessageBus& bus) : mBus(bus)
{
}

void Landscape::setChunkProvider(std::unique_ptr<ChunkProvider> del)
{
    chunkDeliverer = std::move(del);
}

Chunk& Landscape::loadChunk(const ChunkCoordinate& location)
{
    auto chunkIndex = chunkIndices.find(location);
    if(chunkIndex == chunkIndices.end())
    {
        chunks.push_back(chunkDeliverer->fetchChunk(location));
        chunkIndices.emplace(location, chunks.size() - 1);
    }

    return chunks[chunkIndices.at(location)];
}

bool Landscape::chunkIsLoaded(const ChunkCoordinate& location) const
{
    return chunkIndices.find(location) != chunkIndices.end();
}

const Chunk& Landscape::getChunk(const ChunkCoordinate& location) const
{
    return chunks[chunkIndices.at(location)];
}

const std::vector<Chunk>& Landscape::getChunkList() const
{
    return chunks;
}

void Landscape::highlightChunk(size_t id, const ChunkCoordinate& chunk)
{
    checkUnloads(id);

    int32_t halfCheatBoxWidth = 2;

    int32_t centerX = chunk.x;
    int32_t centerY = chunk.y;
    int32_t centerZ = chunk.z;

    for(int32_t x = centerX - halfCheatBoxWidth; x <= centerX + halfCheatBoxWidth; x++)
    {
        for(int32_t y = centerY - halfCheatBoxWidth; y <= centerY + halfCheatBoxWidth; y++)
        {
            for(int32_t z = centerZ - halfCheatBoxWidth; z <= centerZ + halfCheatBoxWidth; z++)
            {
                ChunkCoordinate coordinate(x, y, z);
                mBus.sendMessage<ChunkCreatedMessage>(ChunkCreatedMessage(coordinate, loadChunk(coordinate).getVoxelTypes()));
            }
        }
    }
}

void Landscape::checkUnloads(size_t id)
{
}
