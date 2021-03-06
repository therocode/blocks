#include "highlightcontroller.hpp"
#include "../../world/worldmessages.hpp"

HighlightController::HighlightController(fea::MessageBus& bus) :
    EntityController(bus)
{
    subscribe(mBus, *this);
}

bool HighlightController::keepEntity(fea::WeakEntityPtr entity) const
{
    fea::EntityPtr locked = entity.lock();

    return locked->hasAttribute("highlight_radius") &&
        locked->hasAttribute("is_highlighting");
}

void HighlightController::entityKept(fea::WeakEntityPtr entity)
{
    fea::EntityPtr locked = entity.lock();

    if(locked->getAttribute<bool>("is_highlighting"))
    {
        mBus.send(HighlightEntityAddRequestedMessage{locked->getAttribute<WorldId>("current_world"), locked->getId(), WorldToChunk::convert(locked->getAttribute<glm::vec3>("position")), locked->getAttribute<uint32_t>("highlight_radius")});
    }
}

void HighlightController::entityDropped(fea::WeakEntityPtr entity)
{
    fea::EntityPtr locked = entity.lock();
    if(locked->getAttribute<bool>("is_highlighting"))
    {
        mBus.send(HighlightEntityRemoveRequestedMessage{locked->getAttribute<WorldId>("current_world"), locked->getId()});
    }
}

void HighlightController::handleMessage(const EntityMovedMessage& received)
{
    fea::EntityId id = received.entity->getId();
    if(mEntities.count(id))
    {
        auto entityIterator = mEntities.find(id);

        if(entityIterator != mEntities.end())
        {
            fea::EntityPtr entity = entityIterator->second.lock();

            if(entity->getAttribute<bool>("is_highlighting"))
            {
                ChunkCoord newChunk = WorldToChunk::convert(received.newPosition);

                if(WorldToChunk::convert(received.oldPosition) != newChunk)
                {
                    mBus.send(HighlightEntityMoveRequestedMessage{entity->getAttribute<WorldId>("current_world"), entity->getId(), newChunk});
                }
            }
        }
    }
}

void HighlightController::handleMessage(const EntityTransferredWorldMessage& received)
{
    auto entityIterator = mEntities.find(received.entityId);
    if(entityIterator != mEntities.end())
    {
        fea::EntityPtr entity = entityIterator->second.lock();
        if(entity->getAttribute<bool>("is_highlighting"))
        {
            uint32_t radius = entity->getAttribute<uint32_t>("highlight_radius");
            mBus.send(HighlightEntityRemoveRequestedMessage{received.oldWorld, received.entityId});
            mBus.send(HighlightEntityAddRequestedMessage{received.newWorld, received.entityId, WorldToChunk::convert(mEntities.at(received.entityId).lock()->getAttribute<glm::vec3>("position")), radius});
        }
    }
}
