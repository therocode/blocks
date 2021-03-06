#include "../utilities/glm.hpp"
#include "entityfactory.hpp"
#include "../world/chunk.hpp"
#include "controllers/moveaction.hpp"
#include "controllers/movedirection.hpp"
#include <fea/entity/jsonentityloader.hpp>
#include <fea/entity/glmtypeadder.hpp>


EntityFactory::EntityFactory(fea::EntityManager& manager) : mFactory(manager)
{
    //register templates

    fea::addBasicDataTypes(mFactory);
    fea::addGlmDataTypes(mFactory);
    
    mFactory.addDataType<PhysicsType>("physics_type");
    mFactory.addDataType<MoveAction>("move_action");
    mFactory.addDataType<MoveDirection>("move_direction");

    mFactory.addDataType("quat", [] (const fea::Parameters& params) 
            {
                FEA_ASSERT(params.size() == 4, "Wrong amount of parameters! Expected 4, got " + std::to_string(params.size()) + "!");

                return glm::quat(std::stof(params[0]),std::stof(params[1]),std::stof(params[2]),std::stof(params[3]));
            });
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
    spawned->setAttribute("gfx_entity", definition.gfxEntity);
    spawned->setAttribute<MoveAction>("move_action", MoveAction::STANDING);
    spawned->setAttribute<MoveDirection>("move_direction", MoveDirection(false, false, false, false));

    return spawned;
}

void EntityFactory::addDefinition(const EntityDefinition& definition)
{
    mEntityDefinitions.emplace(definition.name, definition);
}

void EntityFactory::registerAttribute(const std::unordered_map<std::string, std::string> &attribute)
{
	for(const auto& entityAttribute : attribute)
		mFactory.registerAttribute(entityAttribute.first, entityAttribute.second);
}

void EntityFactory::registerEntity(const std::vector<std::pair<std::string, fea::EntityTemplate>> &entity)
{
    for(const auto& entityTemplate : entity)
        mFactory.addTemplate(entityTemplate.first, entityTemplate.second);
}
