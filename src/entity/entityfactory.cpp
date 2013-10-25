#include <featherkit/entitysystemutil.h>
#include <iostream>
#include "../blockstd.h"
#include "entityfactory.h"
#include "defaultsetters.h"

EntityFactory::EntityFactory(fea::EntityManager& manager) : mManager(manager)
{
    //register templates
    fea::util::JsonEntityLoader loader;
    loader.registerType("#vec3#",sizeof(glm::vec3));
    mManager.registerAttributes(loader.loadAttributesJson("data/attributes.json"));
    mManager.registerEntityTemplates(loader.loadEntitiesJson("data/entities.json"));
    mManager.registerDefaultSetter("mass", fea::util::floatSetter);
    mManager.registerDefaultSetter("position", vec3Setter);
    mManager.registerDefaultSetter("velocity", vec3Setter);
    mManager.registerDefaultSetter("acceleration", vec3Setter);
    mManager.registerDefaultSetter("pitch", fea::util::floatSetter);
    mManager.registerDefaultSetter("yaw", fea::util::floatSetter);
    mManager.registerDefaultSetter("hitbox", vec3Setter);
    mManager.registerDefaultSetter("floating", fea::util::boolSetter);
}

fea::WeakEntityPtr EntityFactory::spawnEntity(const std::string& scriptType)
{
    if(mEntityDefinitions.find(scriptType) == mEntityDefinitions.end())
    {
        std::cout << "Error! Trying to spawn entity of type " << scriptType << " but it lacks a definition file\n";
    }

    std::string type = mEntityDefinitions.at(scriptType).category;

    fea::WeakEntityPtr spawned = mManager.createEntity(type);

    //set it all up according to the type;

    return spawned;
}

void EntityFactory::addDefinition(const EntityDefinition& definition)
{
    mEntityDefinitions.emplace(definition.name, definition);
}
