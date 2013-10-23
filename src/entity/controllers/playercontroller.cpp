#include "playercontroller.h"
#include "../../world/worldinterface.h"
#include <iostream>

PlayerController::PlayerController(fea::MessageBus& bus, WorldInterface& worldInterface) : EntityController(bus, worldInterface), mForward(false)
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

void PlayerController::onFrame()
{
    for(auto& throttleEntry : mPlayerThrottles)
    {
        float throttle = throttleEntry.second;
        if(throttle > 0.001f)
        {
            fea::EntityPtr entity = mPlayerEntities.at(throttleEntry.first).lock();
            float pitch = glm::radians(entity->getAttribute<float>("pitch"));
            float yaw = glm::radians(entity->getAttribute<float>("yaw"));

            std::cout << "pitch " << pitch << "\n";
            std::cout << "yaw " << yaw << "\n";

            glm::vec4 speedDir(0.0f, 0.0f, 0.1f, 1.0f);

            glm::mat4 xRot(glm::vec4(glm::cos(yaw), -glm::sin(yaw), 0, 0),
                           glm::vec4(glm::sin(yaw), glm::cos(yaw), 0, 0),
                           glm::vec4(0, 0, 1, 0),
                           glm::vec4(0, 0, 0, 1));

            glm::mat4 yRot(glm::vec4(glm::cos(pitch), 0, glm::sin(pitch), 0),
                           glm::vec4(0, 1, 0, 0),
                           glm::vec4(-glm::sin(pitch), 0, glm::cos(pitch), 0),
                           glm::vec4(0, 0, 0, 1));

            speedDir = xRot * yRot * speedDir;

            entity->addToAttribute("velocity", glm::vec3(speedDir.x, speedDir.y, speedDir.z));
            glm::vec3 vel = entity->getAttribute<glm::vec3>("velocity");
        }
    }
}

void PlayerController::handleMessage(const PlayerJoinedMessage& received)
{
    size_t playerId;
    glm::vec3 position;

    std::tie(playerId, position) = received.data;

    fea::WeakEntityPtr playerEntity = mWorldInterface.spawnEntity("Player", position);
    playerEntity.lock()->setAttribute("floating", true);
    mPlayerEntities.emplace(playerId, playerEntity);
    mPlayerThrottles.emplace(playerId, 0.0f);
}

void PlayerController::handleMessage(const PlayerActionMessage& received)
{
    size_t playerId;
    InputAction action;

    std::tie(playerId, action) = received.data;

    if(action == FORWARDS)
    {
        auto playerEntry = mPlayerThrottles.find(playerId);
        if(playerEntry != mPlayerThrottles.end())
        {
            playerEntry->second = 1.0f;
        }
    }
    else if(action == STOPFORWARDS)
    {
        auto playerEntry = mPlayerThrottles.find(playerId);
        if(playerEntry != mPlayerThrottles.end())
        {
            playerEntry->second = 0.0f;
        }
    }
}

void PlayerController::handleMessage(const PlayerPitchYawMessage& received)
{
 size_t playerId;
    float pitch;
    float yaw;

    std::tie(playerId, pitch, yaw) = received.data;

    auto playerEntry = mPlayerEntities.find(playerId);
    if(playerEntry != mPlayerEntities.end())
    {
        playerEntry->second.lock()->addToAttribute<float>("pitch", pitch);
        playerEntry->second.lock()->addToAttribute<float>("yaw", yaw);
    }
}
