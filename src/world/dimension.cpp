#include "dimension.h"
#include "worldmessages.h"
#include "localchunkprovider.h"

Dimension::Dimension(fea::MessageBus& messageBus) : bus(messageBus)
{

}

void Dimension::initialise()
{
    landscape.setChunkProvider(std::unique_ptr<ChunkProvider>(new LocalChunkProvider()));
}

void Dimension::highlightChunk(size_t id, const ChunkCoordinate& chunk)
{
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
                bus.sendMessage<ChunkCreatedMessage>(ChunkCreatedMessage(coordinate,landscape.loadChunk(coordinate).getVoxelTypes()));
            }
        }
    }
}

const Landscape& Dimension::getLandscape()
{
    return landscape;
}
