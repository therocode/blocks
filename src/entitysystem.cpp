#include "entitysystem.h"
#include "controllers/physicscontroller.h"
#include <featherkit/entitysystemutil.h>

EntitySystem::EntitySystem() : manager(new fea::util::BasicEntityBackend())
{
}

void EntitySystem::initialise()
{
    controllers.push_back(std::unique_ptr<EntityController>(new PhysicsController()));
}

void EntitySystem::update()
{
    for(auto& controller : controllers)
    {
        controller->update();
    }
}

fea::WeakEntityPtr EntitySystem::spawnEntity(const std::string& type)
{
    fea::WeakEntityPtr spawned = manager.createEntity(type);

    for(auto& controller : controllers)
    {
        controller->inspectEntity(spawned);
    }

    return spawned;
}
