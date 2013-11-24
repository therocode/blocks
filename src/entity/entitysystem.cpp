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
    mBus.addMessageSubscriber<CreateEntityMessage>(*this);
    mBus.addMessageSubscriber<RemoveEntityMessage>(*this);
    mTimer.start();
}

EntitySystem::~EntitySystem()
{
    mBus.removeMessageSubscriber<CreateEntityMessage>(*this);
    mBus.removeMessageSubscriber<RemoveEntityMessage>(*this);
}

void EntitySystem::addController(std::unique_ptr<EntityController> controller)
{
    mControllers.push_back(std::move(controller));
}

void EntitySystem::setup()
{
    mBus.sendMessage<LogMessage>(LogMessage("Loading entity definitions", mLogName, LogLevel::INFO));

    EntityDefinitionLoader loader;

    FolderExploder exploder;
    std::vector<std::string> definitionFiles;
    exploder.explodeFolder("data", ".*\\.def", definitionFiles);

    mBus.sendMessage<LogMessage>(LogMessage("Found " + std::to_string(definitionFiles.size()) + " entity definitions", mLogName, LogLevel::INFO));
    for(auto& fileName : definitionFiles)
    {
        EntityDefinition temp = loader.loadFromJSONFile(fileName);
        if(loader.errorOccurred())
        {
            mBus.sendMessage<LogMessage>(LogMessage(loader.getErrorString(), mLogName, LogLevel::ERR));
            mBus.sendMessage<FatalMessage>(std::string("Shutdown issued due to erroneous entity definition"));
        }
        mFactory.addDefinition(temp);
        mBus.sendMessage<LogMessage>(LogMessage("Added entity type '" + temp.name + "' to entity definitions", mLogName, LogLevel::VERB));
    }
    mBus.sendMessage<LogMessage>(LogMessage("Entity definitions loaded", mLogName, LogLevel::INFO));
}

void EntitySystem::update()
{
    int dt = mTimer.getDeltaTime();
    for(auto& controller : mControllers)
    {
        controller->onFrame(dt);
    }
}

fea::WeakEntityPtr EntitySystem::createEntity(const std::string& type, const glm::vec3& position)
{
    fea::WeakEntityPtr entity = mFactory.spawnEntity(type);

    entity.lock()->setAttribute<glm::vec3>("position", position);

    mBus.sendMessage<EntityCreatedMessage>(EntityCreatedMessage(entity, type));

    attachEntity(entity);

    return entity;
}

void EntitySystem::handleMessage(const CreateEntityMessage& received) 
{
    std::string type;
    glm::vec3 position;
    
    std::tie(type, position) = received.data;

    createEntity(type, position);
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
