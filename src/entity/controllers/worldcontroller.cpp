#include "worldcontroller.hpp"
#include "../../world/worldmessages.hpp"

WorldController::WorldController(fea::MessageBus& bus) :
    EntityController(bus)
{
    subscribe(mBus, *this);
}

void WorldController::handleMessage(const EntityWorldTransferRequestedMessage& received)
{
    WorldId oldWorld = received.entity->getAttribute<WorldId>("current_world");

    mBus.send(EntityTransferringWorldMessage{received.entity->getId(), oldWorld, received.worldId});

    received.entity->setAttribute("current_world", received.worldId);

    mBus.send(EntityTransferredWorldMessage{received.entity->getId(), oldWorld, received.worldId});
}
