#include "world.h"
        
World::World(fea::MessageBus& messageBus) : bus(messageBus), standardDimension(messageBus)
{
}

void World::initialise()
{
    standardDimension.initialise();
}

void World::update()
{
}
