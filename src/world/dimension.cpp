#include "dimension.h"
#include "worldmessages.h"
#include "localchunkprovider.h"

Dimension::Dimension(fea::MessageBus& messageBus) : bus(messageBus),
                                                    landscape(messageBus)
{

}

void Dimension::initialise()
{
    landscape.setChunkProvider(std::unique_ptr<ChunkProvider>(new LocalChunkProvider()));
}

void Dimension::highlightChunk(size_t id, const ChunkCoordinate& chunk)
{
    landscape.highlightChunk(id, chunk);
}

const Landscape& Dimension::getLandscape()
{
    return landscape;
}
