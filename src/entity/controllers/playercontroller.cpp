#include "playercontroller.h"
#include "../../world/worldinterface.h"
#include <iostream>

PlayerController::PlayerController(fea::MessageBus& bus, WorldInterface& worldInterface) : EntityController(bus, worldInterface)
{
    mBus.addMessageSubscriber<PlayerJoinedMessage>(*this);
    mBus.addMessageSubscriber<PlayerActionMessage>(*this);
    mBus.addMessageSubscriber<PlayerPitchYawMessage>(*this);
}

PlayerController::~PlayerController()
{
    mBus.removeMessageSubscriber<PlayerJoinedMessage>(*this);
    mBus.removeMessageSubscriber<PlayerActionMessage>(*this);
    mBus.removeMessageSubscriber<PlayerPitchYawMessage>(*this);
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

    fea::WeakEntityPtr playerEntity = mWorldInterface.spawnEntity("Player", position);
    playerEntity.lock()->setAttribute<bool>("floating", true);
    mPlayerEntities.emplace(playerId, playerEntity);
}

void PlayerController::handleMessage(const PlayerActionMessage& received)
{
    size_t playerId;
    InputAction action;

    std::tie(playerId, action) = received.data;

    //if(action == 
}

void PlayerController::handleMessage(const PlayerPitchYawMessage& received)
{
    size_t playerId;
    float pitch;
    float yaw;

    std::tie(playerId, pitch, yaw) = received.data;

    //if(action == 
}
