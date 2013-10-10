#include "dimension.h"
#include "chunkgenerator.h"
#include "messages.h"
#include "localchunkdeliverer.h"

Dimension::Dimension(fea::MessageBus& messageBus) : bus(messageBus)
{

}

void Dimension::initialise()
{
    landscape.setChunkDeliverer(new LocalChunkDeliverer());

    ChunkCoordinate coordinate(0,0,0);
    
    bus.sendMessage<ChunkCreatedMessage>(ChunkCreatedMessage(&coordinate,&landscape.loadChunk(coordinate)));
}
