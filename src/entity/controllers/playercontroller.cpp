#include "playercontroller.h"
#include "../../world/worldinterface.h"
#include "../../rendering/renderingmessages.h"
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
        //if(throttle > 0.001f)
        {
            fea::EntityPtr entity = mPlayerEntities.at(throttleEntry.first).lock();
            float pitch = entity->getAttribute<float>("pitch");
            float yaw = entity->getAttribute<float>("yaw");

			if(pitch >= glm::pi<float>() * 0.5f)
				pitch = glm::pi<float>() * 0.5f - 0.001f;
			if(pitch <= -glm::pi<float>() * 0.5f)
				pitch = -glm::pi<float>() * 0.5f + 0.001f;
			
			//printf("direction1:%f, %f, %f\n", pitch, yaw,0);
           // std::cout << "pitch " << pitch << "\n";
            //std::cout << "yaw " << yaw << "\n";
			glm::vec3 currentSpeed = entity->getAttribute<glm::vec3>("velocity");
            glm::vec3 speedDir(0.0f, 0.0f, 1.0f);
			
            glm::mat3 xRot( glm::vec3(1, 			0, 			0),
							glm::vec3(0, glm::cos(pitch), -glm::sin(pitch)),
							glm::vec3(0, glm::sin(pitch), glm::cos(pitch))
                          );
			
            glm::mat3 yRot(glm::vec3(glm::cos(yaw), 0, glm::sin(yaw)),
                           glm::vec3(0, 1, 0),
                           glm::vec3(-glm::sin(yaw), 0, glm::cos(yaw))
                          );
			
            currentSpeed = glm::vec3(glm::cos(pitch)*glm::sin(yaw), glm::sin(pitch), glm::cos(pitch) * glm::cos(yaw)) * 0.5f;//glm::vec3(yRot * xRot * speedDir);
            entity->setAttribute("velocity", currentSpeed);
            glm::vec3 vel = entity->getAttribute<glm::vec3>("velocity");
        }
    }

    for(auto& chunkEntry : mPlayerChunks)
    {
        fea::EntityPtr entity = mPlayerEntities.at(chunkEntry.first).lock();

        glm::vec3 position = entity->getAttribute<glm::vec3>("position");

        if(worldToChunk(position) != chunkEntry.second)
        {
            playerEntersChunk(chunkEntry.first, worldToChunk(position));
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
    playerEntersChunk(playerId, worldToChunk(position));

    mBus.sendMessage<PlayerConnectedToEntityMessage>(PlayerConnectedToEntityMessage(playerId, playerEntity.lock()->getId()));
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
        fea::EntityPtr entity = playerEntry->second.lock();
		float newPitch = entity->getAttribute<float>("pitch");
		newPitch += glm::radians(pitch);
		
		if(newPitch >= glm::pi<float>() * 0.5f)
			newPitch = glm::pi<float>() * 0.5f - 0.001f;
		if(newPitch <= -glm::pi<float>() * 0.5f)
			newPitch = -glm::pi<float>() * 0.5f + 0.001f;
		
		
			
        entity->setAttribute<float>("pitch", newPitch);
        entity->addToAttribute<float>("yaw",   glm::radians(yaw));

        float newYaw = entity->getAttribute<float>("yaw");
		//printf("Pitch: %f, and yaw: %f\n", newPitch, newYaw);
        mBus.sendMessage<RotateGfxEntityMessage>(RotateGfxEntityMessage(playerEntry->second.lock()->getId(), newPitch, newYaw));
    }
}

void PlayerController::playerEntersChunk(size_t playerId, const ChunkCoordinate& chunk)
{
    mBus.sendMessage<PlayerEntersChunkMessage>(PlayerEntersChunkMessage(playerId, chunk));
    mPlayerChunks[playerId] = chunk;
}
