#include "entitysystem.h"
#include "../blockstd.h"
#include "entitymessages.h"
#include <featherkit/entitysystemutil.h>

EntitySystem::EntitySystem(fea::MessageBus& bus) : 
    mBus(bus), 
    mManager(new fea::util::BasicEntityBackend()),
    mFactory(mManager)
{
    mBus.addMessageSubscriber<SpawnEntityMessage>(*this);
    mBus.addMessageSubscriber<RemoveEntityMessage>(*this);
}

EntitySystem::~EntitySystem()
{
    mBus.removeMessageSubscriber<SpawnEntityMessage>(*this);
    mBus.removeMessageSubscriber<RemoveEntityMessage>(*this);
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

    mBus.sendMessage<EntityNeedsScriptMessage>(EntityNeedsScriptMessage(entity, scriptType));
    mBus.sendMessage<EntitySpawnedMessage>(EntitySpawnedMessage(entity, scriptType));

    attachEntity(entity);

    return entity;
}

void EntitySystem::spawnEntityFromScriptHandle(const std::string& scriptType, const glm::vec3& position, asIScriptObject* obj)
{
    fea::WeakEntityPtr entity = mFactory.spawnEntity(scriptType);

    entity.lock()->setAttribute<glm::vec3>("position", position);

    mBus.sendMessage<ScriptEntityFinishedMessage>(ScriptEntityFinishedMessage(entity.lock()->getId(), obj, entity));
    mBus.sendMessage<EntitySpawnedMessage>(EntitySpawnedMessage(entity, scriptType));

    attachEntity(entity);
}

void EntitySystem::handleMessage(const SpawnEntityMessage& received) 
{
    std::string scriptType;
    glm::vec3 position;
    
    std::tie(scriptType, position) = received.data;

    spawnEntity(scriptType, position);
}

void EntitySystem::handleMessage(const RemoveEntityMessage& received)
{
    size_t id;

    std::tie(id) = received.data;

    removeEntity(id);
}

void EntitySystem::attachEntity(fea::WeakEntityPtr entity)
{
    for(auto& controller : mControllers)
    {
        controller->inspectEntity(entity);
    }
}

void EntitySystem::removeEntity(fea::EntityId id)
{
    for(auto& controller : mControllers)
    {
        controller->removeEntity(id);
    }

    mManager.removeEntity(id);

    mBus.sendMessage<EntityRemovedMessage>(EntityRemovedMessage(id));
}
