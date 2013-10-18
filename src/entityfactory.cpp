#include "entityfactory.h"

EntityFactory::EntityFactory(fea::EntityManager& manager) : mManager(manager)
{
    //register templates
}

fea::WeakEntityPtr EntityFactory::spawnEntity(const std::string& scriptType)
{
    std::string type = "human";  //work out underlying type from script type

    fea::WeakEntityPtr spawned = mManager.createEntity(type);

    //set it all up according to the type;

    return spawned;
}
