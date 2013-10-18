#include "world.h"
#include "controllers/physicscontroller.h"
#include "controllers/collisioncontroller.h"
#include "controllers/gfxcontroller.h"

	World::World(fea::MessageBus& messageBus) 
:   bus(messageBus),
	standardDimension(messageBus),
	entitySystem(messageBus),
	worldInterface(standardDimension, entitySystem)
{
	bus.addMessageSubscriber<CameraUpdatedMessage>(*this);
}

World::~World()
{
	bus.removeMessageSubscriber<CameraUpdatedMessage>(*this);
}
void World::initialise()
{
	standardDimension.initialise();

	standardDimension.addFocusPoint(FocusPoint(glm::vec3(0.0f, 0.0f, 0.0f), 4.0f));

	entitySystem.addController(std::unique_ptr<EntityController>(new PhysicsController(bus, worldInterface)));
	entitySystem.addController(std::unique_ptr<EntityController>(new CollisionController(bus, worldInterface)));
	entitySystem.addController(std::unique_ptr<EntityController>(new GfxController(bus, worldInterface)));

	for(int x = 0; x < 2; x++)
	{
		for(int y = 0; y < 2; y++)
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
