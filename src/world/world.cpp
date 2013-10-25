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
	bus.addMessageSubscriber<PlayerEntersChunkMessage>(*this);
}

World::~World()
{
	bus.removeMessageSubscriber<PlayerEntersChunkMessage>(*this);
}

void World::initialise()
{
	standardDimension.initialise();

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
}

void World::handleMessage(const PlayerEntersChunkMessage& received)
{
    size_t playerId;
    ChunkCoordinate chunkCoordinate;

	std::tie(playerId, chunkCoordinate) = received.data;

    standardDimension.highlightChunk(playerId, chunkCoordinate);
}

WorldInterface& World::getWorldInterface()
{
    return worldInterface;
}

