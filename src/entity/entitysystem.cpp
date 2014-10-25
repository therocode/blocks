#include "entitysystem.hpp"
#include "../utilities/glm.hpp"
#include "entitymessages.hpp"
#include "../application/applicationmessages.hpp"
#include "entitydefinitionloader.hpp"
#include "../utilities/folderexploder.hpp"
#include "controllers/entitycontroller.hpp"
#include "../lognames.hpp"

EntitySystem::EntitySystem(fea::MessageBus& bus) : 
    mBus(bus), 
    mManager(),
    mFactory(mManager)
{
    subscribe(mBus, *this);
    mBus.send(LogMessage{"Setting up entity system", entityName, LogLevel::INFO});

    mBus.send(LogMessage{"Loading entity definitions", entityName, LogLevel::INFO});

    EntityDefinitionLoader loader;

    FolderExploder exploder;
    std::vector<std::string> definitionFiles;
    exploder.explodeFolder("data", ".*\\.def", definitionFiles);

    mBus.send(LogMessage{"Found " + std::to_string(definitionFiles.size()) + " entity definitions", entityName, LogLevel::INFO});
    for(auto& fileName : definitionFiles)
    {
        EntityDefinition temp = loader.loadFromJSONFile(fileName);
        if(loader.errorOccurred())
        {
            mBus.send(LogMessage{loader.getErrorString(), entityName, LogLevel::ERR});
        }
        else
        {
            mFactory.addDefinition(temp);
            mBus.send(LogMessage{"Added entity type '" + temp.name + "' to entity definitions", entityName, LogLevel::VERB});
        }
    }
    mBus.send(LogMessage{"Entity definitions loaded", entityName, LogLevel::INFO});
}

EntitySystem::~EntitySystem()
{
    mBus.send(LogMessage{"Removing all entities", entityName, LogLevel::VERB});
    fea::EntitySet entities = mManager.getAll();

    for(auto entity : entities)
    {
        removeEntity(entity.lock()->getId());
    }
    mBus.send(LogMessage{"Shutting down entity system", entityName, LogLevel::INFO});
}

void EntitySystem::addController(std::unique_ptr<fea::EntityComponent> controller)
{
    mControllers.push_back(std::move(controller));
}

void EntitySystem::handleMessage(const EntityRequestedMessage& received) 
{
    createEntity(received.type, received.initializer);
}

void EntitySystem::handleMessage(const RemoveEntityRequestedMessage& received)
{
    removeEntity(received.id);
}

const fea::EntityManager& EntitySystem::getEntityManager() const
{
    return mManager;
}

fea::WeakEntityPtr EntitySystem::createEntity(const std::string& type, std::function<void(fea::EntityPtr)> initializer)
{
    fea::WeakEntityPtr e = mFactory.spawnEntity(type);

    if(fea::EntityPtr entity = e.lock())
    {
        initializer(entity);

        mBus.send(EntityCreatedMessage{entity, type});

        attachEntity(entity);
    }
    else
    {
        mBus.send(LogMessage{"Trying to spawn entity of invalid type " + type, entityName, LogLevel::ERR});
    }

    return e;
}

void EntitySystem::attachEntity(fea::WeakEntityPtr entity)
{
    for(auto& controller : mControllers)
    {
        controller->entityCreated(entity);
    }
}

void EntitySystem::removeEntity(fea::EntityId id)
{
    for(auto& controller : mControllers)
    {
        controller->entityRemoved(id);
    }

    mManager.removeEntity(id);

    mBus.send(EntityRemovedMessage{id});
}
