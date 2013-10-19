#include <featherkit/entitysystemutil.h>
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
    mManager.registerDefaultSetter("hitbox", vec3Setter);
}

fea::WeakEntityPtr EntityFactory::spawnEntity(const std::string& scriptType)
{
    std::string type = "human";  //work out underlying type from script type

    fea::WeakEntityPtr spawned = mManager.createEntity(type);

    //set it all up according to the type;

    return spawned;
}
