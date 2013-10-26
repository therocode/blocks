#include "entitysystem.h"
#include "../blockstd.h"
#include "entitymessages.h"
#include "../application/applicationmessages.h"
#include "entitydefinitionloader.h"
#include "../utilities/folderexploder.h"
#include <featherkit/entitysystemutil.h>

EntitySystem::EntitySystem(fea::MessageBus& bus) : 
    mBus(bus), 
    mManager(new fea::util::BasicEntityBackend()),
    mFactory(mManager),
    mLogName("entity")
{
    mBus.addMessageSubscriber<SpawnEntityMessage>(*this);
    mBus.addMessageSubscriber<RemoveEntityMessage>(*this);

    EntityDefinitionLoader loader;

    FolderExploder exploder;
    std::vector<std::string> definitionFiles;
    exploder.explodeFolder("data", ".*\\.def", definitionFiles);

    for(auto& fileName : definitionFiles)
    {
        EntityDefinition temp = loader.loadFromJSONFile(fileName);
        mFactory.addDefinition(temp);
        mBus.sendMessage<LogMessage>(LogMessage("Added entity type '" + temp.name + "' to entity definitions", mLogName, LogLevel::VERB));
    }
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
        controller->onFrame();
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

size_t EntitySystem::spawnEntityFromScriptHandle(const std::string& scriptType, const glm::vec3& position, asIScriptObject* obj)
{
    fea::WeakEntityPtr entity = mFactory.spawnEntity(scriptType);

    entity.lock()->setAttribute<glm::vec3>("position", position);

    mBus.sendMessage<ScriptEntityFinishedMessage>(ScriptEntityFinishedMessage(entity.lock()->getId(), obj, entity));
    mBus.sendMessage<EntitySpawnedMessage>(EntitySpawnedMessage(entity, scriptType));

    attachEntity(entity);

    return entity.lock()->getId();
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
