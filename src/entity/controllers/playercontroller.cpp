#include "playercontroller.h"
#include "../../world/worldinterface.h"
#include <iostream>

PlayerController::PlayerController(fea::MessageBus& bus, WorldInterface& worldInterface) : EntityController(bus, worldInterface)
{
    mBus.addMessageSubscriber<PlayerJoinedMessage>(*this);
}

PlayerController::~PlayerController()
{
    mBus.removeMessageSubscriber<PlayerJoinedMessage>(*this);
}

void PlayerController::inspectEntity(fea::WeakEntityPtr entity)
{
}

void PlayerController::removeEntity(fea::EntityId id)
{
}

void PlayerController::handleMessage(const PlayerJoinedMessage& received)
{
    size_t playerId;
    glm::vec3 position;

    std::tie(playerId, position) = received.data;

    fea::WeakEntityPtr playerEntity = mWorldInterface.spawnEntity("Elephant", position);
    std::cout << "spawned a good one\n";

    mPlayerEntities.emplace(playerId, playerEntity);
}
