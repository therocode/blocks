#include "world.h"
        
World::World(fea::MessageBus& messageBus) : bus(messageBus), standardDimension(messageBus)
{
}

void World::initialise()
{
    standardDimension.initialise();

    standardDimension.addFocusPoint(FocusPoint(glm::vec3(0.0f, 0.0f, 0.0f), 2.0f));
}

void World::update()
{
}
