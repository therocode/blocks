#include "entitysystem.h"
#include "controllers/physicscontroller.h"
#include "controllers/collisioncontroller.h"
#include "controllers/gfxcontroller.h"
#include "blockstd.h"
#include "defaultsetters.h"
#include <featherkit/entitysystemutil.h>

EntitySystem::EntitySystem(fea::MessageBus& b) : bus(b), manager(new fea::util::BasicEntityBackend())
{
}

void EntitySystem::initialise()
{
    controllers.push_back(std::unique_ptr<EntityController>(new PhysicsController(bus)));
    controllers.push_back(std::unique_ptr<EntityController>(new CollisionController(bus)));
    controllers.push_back(std::unique_ptr<EntityController>(new GfxController(bus)));

    fea::util::JsonEntityLoader loader;
    loader.registerType("#vec3#",sizeof(glm::vec3));
    manager.registerAttributes(loader.loadAttributesJson("data/attributes.json"));
    manager.registerEntityTemplates(loader.loadEntitiesJson("data/entities.json"));
    manager.registerDefaultSetter("mass", fea::util::floatSetter);
    manager.registerDefaultSetter("position", vec3Setter);
    manager.registerDefaultSetter("velocity", vec3Setter);
    manager.registerDefaultSetter("acceleration", vec3Setter);
    manager.registerDefaultSetter("hitbox", vec3Setter);
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
    return manager.createEntity(type);
}

void EntitySystem::attachEntity(fea::WeakEntityPtr entity)
{
    for(auto& controller : controllers)
    {
        controller->inspectEntity(entity);
    }
}
