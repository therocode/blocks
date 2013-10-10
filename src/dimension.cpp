#include "dimension.h"
#include "messages.h"
#include "localchunkdeliverer.h"

Dimension::Dimension(fea::MessageBus& messageBus) : bus(messageBus)
{

}

void Dimension::initialise()
{
    landscape.setChunkDeliverer(new LocalChunkDeliverer());
}

void Dimension::addFocusPoint(const FocusPoint& focusPoint)
{
    int32_t halfCheatBoxWidth = (focusPoint.radius + 1) / 2;

    int32_t centerX = floor(focusPoint.position.x / (float)chunkWidth);
    int32_t centerY = floor(focusPoint.position.y / (float)chunkWidth);
    int32_t centerZ = floor(focusPoint.position.z / (float)chunkWidth);

    for(int32_t x = centerX - halfCheatBoxWidth; x <= centerX + halfCheatBoxWidth; x++)
    {
        for(int32_t y = centerY - halfCheatBoxWidth; y <= centerY + halfCheatBoxWidth; y++)
        {
            for(int32_t z = centerZ - halfCheatBoxWidth; z <= centerZ + halfCheatBoxWidth; z++)
            {
                ChunkCoordinate coordinate(x, y, z);
                bus.sendMessage<ChunkCreatedMessage>(ChunkCreatedMessage(&coordinate,&landscape.loadChunk(coordinate)));
            }
        }
    }
}
