#include "highlightcontroller.hpp"
#include "../../world/worldmessages.hpp"

HighlightController::HighlightController(fea::MessageBus& bus, GameInterface& gameInterface) :
    EntityController(bus, gameInterface)
{
    subscribe(mBus, *this);
}

void HighlightController::inspectEntity(fea::WeakEntityPtr entity)
{
    fea::EntityPtr locked = entity.lock();

    if(locked->hasAttribute("highlight_radius") &&
            locked->hasAttribute("highlighting"))
    {
        mEntities.emplace(locked->getId(), entity);
    }
}

void HighlightController::removeEntity(fea::EntityId id)
{
}

void HighlightController::handleMessage(const EntityMovedMessage& received)
{
    size_t id = received.entityId;

    auto entityIterator = mEntities.find(id);

    if(entityIterator != mEntities.end())
    {
        fea::EntityPtr entity = entityIterator->second.lock();

        ChunkCoord newChunk = WorldToChunk::convert(received.newPosition);

        if(WorldToChunk::convert(received.oldPosition) != newChunk)
        {
            //send highlight
        }
    }

}
