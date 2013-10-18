#include "entitysystem.h"
#include "blockstd.h"
#include "defaultsetters.h"
#include "messages.h"
#include <featherkit/entitysystemutil.h>

EntitySystem::EntitySystem(fea::MessageBus& bus) : 
    mBus(bus), 
    mManager(new fea::util::BasicEntityBackend()),
    mFactory(mManager)
{
    mBus.addMessageSubscriber<SpawnEntityMessage>(*this);
}

EntitySystem::~EntitySystem()
{
    mBus.removeMessageSubscriber<SpawnEntityMessage>(*this);
}

void EntitySystem::initialise()
{
    fea::util::JsonEntityLoader loader;
    loader.registerType("#vec3#",sizeof(glm::vec3));
    mManager.registerAttributes(loader.loadAttributesJson("data/attributes.json"));
    mManager.registerEntityTemplates(loader.loadEntitiesJson("data/entities.json"));
    mManager.registerDefaultSetter("mass", fea::util::floatSetter);
    mManager.registerDefaultSetter("position", vec3Setter);
    mManager.registerDefaultSetter("velocity", vec3Setter);
    mManager.registerDefaultSetter("acceleration", vec3Setter);
    mManager.registerDefaultSetter("hitbox", vec3Setter);
}

void EntitySystem::addController(std::unique_ptr<EntityController> controller)
{
    mControllers.push_back(std::move(controller));
}

void EntitySystem::update()
{
    for(auto& controller : mControllers)
    {
        controller->update();
    }
}

fea::WeakEntityPtr EntitySystem::spawnEntity(const std::string& scriptType, const glm::vec3& position)
{
    fea::WeakEntityPtr entity = mFactory.spawnEntity(scriptType);

    entity.lock()->setAttribute<glm::vec3>("position", position);

    mBus.sendMessage<EntitySpawnedMessage>(EntitySpawnedMessage(entity, scriptType));

    attachEntity(entity);

    return entity;
}

void EntitySystem::attachEntity(fea::WeakEntityPtr entity)
{
    for(auto& controller : mControllers)
    {
        controller->inspectEntity(entity);
    }
}

void EntitySystem::handleMessage(const SpawnEntityMessage& received) 
{
    std::string scriptType;
    glm::vec3 position;
    
    std::tie(scriptType, position) = received.data;

    spawnEntity(scriptType, position);
}
