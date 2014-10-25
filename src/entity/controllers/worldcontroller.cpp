#include "worldcontroller.hpp"
#include "../../world/worldmessages.hpp"

WorldController::WorldController(fea::MessageBus& bus) :
    EntityController(bus)
{
    subscribe(mBus, *this);
}

void WorldController::handleMessage(const EntityWorldTransferRequestedMessage& received)
{
    std::cout << received.entity->getId() << " wants to enter world " << received.worldId << "\n";

    WorldId oldWorld = received.entity->getAttribute<WorldId>("current_world");
    received.entity->setAttribute("current_world", received.worldId);

    mBus.send(EntityTransferredWorldMessage{received.entity->getId(), oldWorld, received.worldId});
}
