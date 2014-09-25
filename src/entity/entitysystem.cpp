#include "entitysystem.hpp"
#include "../blockstd.hpp"
#include "entitymessages.hpp"
#include "../application/applicationmessages.hpp"
#include "entitydefinitionloader.hpp"
#include "../utilities/folderexploder.hpp"

EntitySystem::EntitySystem(fea::MessageBus& bus) : 
    mBus(bus), 
    mManager(),
    mFactory(mManager),
    mLogName("entity")
{
    subscribe(mBus, *this);
    mTimer.start();
}

EntitySystem::~EntitySystem()
{
    mBus.send<LogMessage>(LogMessage{"Removing all entities", mLogName, LogLevel::VERB});
    fea::EntitySet entities = mManager.getAll();

    for(auto entity : entities)
    {
        removeEntity(entity.lock()->getId());
    }
}

void EntitySystem::addController(std::unique_ptr<EntityController> controller)
{
    mControllers.push_back(std::move(controller));
}

void EntitySystem::setup()
{
    mBus.send<LogMessage>(LogMessage{"Loading entity definitions", mLogName, LogLevel::INFO});

    EntityDefinitionLoader loader;

    FolderExploder exploder;
    std::vector<std::string> definitionFiles;
    exploder.explodeFolder("data", ".*\\.def", definitionFiles);

    mBus.send<LogMessage>(LogMessage{"Found " + std::to_string(definitionFiles.size()) + " entity definitions", mLogName, LogLevel::INFO});
    for(auto& fileName : definitionFiles)
    {
        EntityDefinition temp = loader.loadFromJSONFile(fileName);
        if(loader.errorOccurred())
        {
            mBus.send(LogMessage{loader.getErrorString(), mLogName, LogLevel::ERR});
            mBus.send(FatalMessage{std::string("Shutdown issued due to erroneous entity definition")});
        }
        mFactory.addDefinition(temp);
        mBus.send<LogMessage>(LogMessage{"Added entity type '" + temp.name + "' to entity definitions", mLogName, LogLevel::VERB});
    }
    mBus.send<LogMessage>(LogMessage{"Entity definitions loaded", mLogName, LogLevel::INFO});
}

void EntitySystem::update()
{
    int dt = mTimer.getDeltaTime();
    for(auto& controller : mControllers)
    {
        controller->onFrame(dt);
    }
}

fea::WeakEntityPtr EntitySystem::createEntity(const std::string& type, std::function<void(fea::EntityPtr)> initializer)
{
    fea::WeakEntityPtr entity = mFactory.spawnEntity(type);

    initializer(entity.lock());

    mBus.send<EntityCreatedMessage>(EntityCreatedMessage{entity, type});

    attachEntity(entity);

    return entity;
}

void EntitySystem::handleMessage(const CreateEntityMessage& received) 
{
    createEntity(received.type, received.initializer);
}

void EntitySystem::handleMessage(const RemoveEntityMessage& received)
{
    removeEntity(received.id);
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

    mBus.send<EntityRemovedMessage>(EntityRemovedMessage{id});
}

EntityCreator EntitySystem::getEntityCreator()
{
    return std::bind(&EntitySystem::createEntity, this, std::placeholders::_1, std::placeholders::_2);
}
