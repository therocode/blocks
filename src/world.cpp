#include "world.h"
        
World::World(fea::MessageBus& messageBus) 
    :   bus(messageBus),
        standardDimension(messageBus),
        worldInterface(standardDimension)
{
}

void World::initialise()
{
    standardDimension.initialise();

    standardDimension.addFocusPoint(FocusPoint(glm::vec3(0.0f, 0.0f, 0.0f), 4.0f));

    entitySystem.initialise();
}

void World::update()
{
}
