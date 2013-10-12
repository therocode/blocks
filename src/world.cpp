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
}

void World::initialise()
{
    standardDimension.initialise();

    standardDimension.addFocusPoint(FocusPoint(glm::vec3(0.0f, 0.0f, 0.0f), 4.0f));

    entitySystem.initialise();
    entitySystem.addController(std::unique_ptr<EntityController>(new PhysicsController(bus, worldInterface)));
    entitySystem.addController(std::unique_ptr<EntityController>(new CollisionController(bus, worldInterface)));
    entitySystem.addController(std::unique_ptr<EntityController>(new GfxController(bus, worldInterface)));


    fea::EntityPtr human = entitySystem.spawnEntity("human").lock();
    human->setAttribute<glm::vec3>("position", glm::vec3(0.0f, 100.0f, 0.0f));
    entitySystem.attachEntity(human);
}

void World::update()
{
    entitySystem.update();
}
