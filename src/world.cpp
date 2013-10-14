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

	entitySystem.initialise();
	entitySystem.addController(std::unique_ptr<EntityController>(new PhysicsController(bus, worldInterface)));
	entitySystem.addController(std::unique_ptr<EntityController>(new CollisionController(bus, worldInterface)));
	entitySystem.addController(std::unique_ptr<EntityController>(new GfxController(bus, worldInterface)));

	for(int x = -10; x < 48; x++)
	{
		for(int y = -10; y < 48; y++)
		{
			fea::EntityPtr human = entitySystem.spawnEntity("human").lock();
			human->setAttribute<glm::vec3>("position", glm::vec3(1.0f * x, 100.0f + glm::linearRand(0.f, 80.f), 1.0f * y));
			entitySystem.attachEntity(human);
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
