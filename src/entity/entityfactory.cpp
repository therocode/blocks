#include "../blockstd.hpp"
#include "entityfactory.hpp"
#include "../world/chunk.hpp"
#include "controllers/moveaction.hpp"
#include "controllers/movedirection.hpp"
#include <fea/entity/jsonentityloader.hpp>
#include <fea/entity/glmtypeadder.hpp>


EntityFactory::EntityFactory(fea::EntityManager& manager) : mFactory(manager)
{
    //register templates
    fea::JsonEntityLoader loader;

    fea::addBasicDataTypes(mFactory);
    fea::addGlmDataTypes(mFactory);
    
    mFactory.addDataType<PhysicsType>("physics_type");
    mFactory.addDataType<MoveAction>("move_action");
    mFactory.addDataType<MoveDirection>("move_direction");

    for(const auto& entityAttribute : loader.loadEntityAttributes("data/attributes.json"))
        mFactory.registerAttribute(entityAttribute.first, entityAttribute.second);

    for(const auto& entityTemplate : loader.loadEntityTemplates("data/entities.json"))
        mFactory.addTemplate(entityTemplate.first, entityTemplate.second);
}

fea::WeakEntityPtr EntityFactory::spawnEntity(const std::string& scriptType)
{
    if(mEntityDefinitions.find(scriptType) == mEntityDefinitions.end())
    {
        return fea::WeakEntityPtr();
    }

    const EntityDefinition& definition = mEntityDefinitions.at(scriptType);
    std::string category = definition.category;

    fea::EntityPtr spawned = mFactory.instantiate(category).lock();
    spawned->setAttribute<PhysicsType>("physics_type", definition.physicsType);
    spawned->setAttribute<float>("drag", definition.drag);
    spawned->setAttribute<float>("walk_speed", definition.walkSpeed);
    spawned->setAttribute<float>("run_speed", definition.runSpeed);
    spawned->setAttribute<float>("jump_strength", definition.jumpStrength);
    spawned->setAttribute<MoveAction>("move_action", MoveAction::STANDING);
    spawned->setAttribute<MoveDirection>("move_direction", MoveDirection(false, false, false, false));

    return spawned;
}

void EntityFactory::addDefinition(const EntityDefinition& definition)
{
    mEntityDefinitions.emplace(definition.name, definition);
}
