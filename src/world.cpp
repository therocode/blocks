#include "world.h"
        
World::World(fea::MessageBus& messageBus) 
    :   bus(messageBus),
        standardDimension(messageBus),
        worldInterface(standardDimension),
        entitySystem(messageBus)
{
}

void World::initialise()
{
    standardDimension.initialise();

    standardDimension.addFocusPoint(FocusPoint(glm::vec3(0.0f, 0.0f, 0.0f), 4.0f));

    entitySystem.initialise();

    fea::EntityPtr human = entitySystem.spawnEntity("human").lock();
    human->setAttribute<glm::vec3>("position", glm::vec3(0.0f, 100.0f, 0.0f));
    entitySystem.attachEntity(human);
}

void World::update()
{
    entitySystem.update();
}
