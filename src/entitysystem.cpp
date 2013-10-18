#include "entitysystem.h"
#include "blockstd.h"
#include "defaultsetters.h"
#include "messages.h"
#include <featherkit/entitysystemutil.h>

EntitySystem::EntitySystem(fea::MessageBus& b) : bus(b), manager(new fea::util::BasicEntityBackend())
{
}

void EntitySystem::initialise()
{
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

void EntitySystem::addController(std::unique_ptr<EntityController> controller)
{
    controllers.push_back(std::move(controller));
}

void EntitySystem::update()
{
    for(auto& controller : controllers)
    {
        controller->update();
    }
}

fea::WeakEntityPtr EntitySystem::spawnEntity(const std::string& type, const std::string& scriptType)
{
    fea::WeakEntityPtr spawned = manager.createEntity(type);
    bus.sendMessage<EntitySpawnedMessage>(EntitySpawnedMessage(spawned, scriptType));
    return spawned;
}

void EntitySystem::attachEntity(fea::WeakEntityPtr entity)
{
    for(auto& controller : controllers)
    {
        controller->inspectEntity(entity);
    }
}
