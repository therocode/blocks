#include "world.h"
#include "../entity/controllers/playercontroller.h"
#include "../entity/controllers/physicscontroller.h"
#include "../entity/controllers/collisioncontroller.h"
#include "../entity/controllers/gfxcontroller.h"

	World::World(fea::MessageBus& messageBus) 
:   bus(messageBus),
	standardDimension(messageBus),
	entitySystem(messageBus),
	worldInterface(standardDimension, entitySystem)
{
	bus.addMessageSubscriber<CameraUpdatedMessage>(*this);
	bus.addMessageSubscriber<PlayerEntersChunkMessage>(*this);
}

World::~World()
{
	bus.removeMessageSubscriber<CameraUpdatedMessage>(*this);
	bus.removeMessageSubscriber<PlayerEntersChunkMessage>(*this);
}

void World::initialise()
{
	standardDimension.initialise();

	standardDimension.addFocusPoint(FocusPoint(glm::vec3(0.0f, 0.0f, 0.0f), 4.0f));

	entitySystem.addController(std::unique_ptr<EntityController>(new PlayerController(bus, worldInterface)));
	entitySystem.addController(std::unique_ptr<EntityController>(new PhysicsController(bus, worldInterface)));
	entitySystem.addController(std::unique_ptr<EntityController>(new CollisionController(bus, worldInterface)));
	entitySystem.addController(std::unique_ptr<EntityController>(new GfxController(bus, worldInterface)));

	for(int x = 0; x < 4; x++)
	{
		for(int y = 0; y < 4; y++)
		{
            bus.sendMessage<SpawnEntityMessage>(SpawnEntityMessage("Elephant", glm::vec3(1.0f * x + 0.5f, 100.0f + glm::linearRand(0.f, 80.f), 1.0f * y + 0.5f)));
		}
	}

}

void World::update()
{
	entitySystem.update();
	glm::vec3 d = worldInterface.getVoxelAtRay(mCamPos, mCamDir);
	bus.sendMessage<CurrentlyFacingBlockMessage>(CurrentlyFacingBlockMessage(d));

}

void World::handleMessage(const CameraUpdatedMessage& received)
{
	glm::vec3 pos, dir;
	std::tie(pos, dir) = received.data;
	mCamPos = pos;
	mCamDir = dir;
}

void World::handleMessage(const PlayerEntersChunkMessage& received)
{
    size_t playerId;
    ChunkCoordinate chunkCoordinate;

	std::tie(playerId, chunkCoordinate) = received.data;

    standardDimension.addFocusPoint(FocusPoint(glm::vec3(chunkCoordinate.x * 16.0f, chunkCoordinate.y * 16.0f, chunkCoordinate.z * 16.0f), 4.0f));
}

WorldInterface& World::getWorldInterface()
{
    return worldInterface;
}
