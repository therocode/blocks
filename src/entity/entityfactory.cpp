#include <fea/entitysystemutil.h>
#include <iostream>
#include "../blockstd.h"
#include "entityfactory.h"
#include "defaultsetters.h"
#include "../world/chunk.h"
#include "controllers/moveaction.h"
#include "controllers/movedirection.h"

EntityFactory::EntityFactory(fea::EntityManager& manager) : mManager(manager), mFactory(manager)
{
    //register templates
    fea::util::JsonEntityLoader loader;
    mManager.registerAttribute<glm::vec3>("position");
    mManager.registerAttribute<glm::vec3>("velocity");
    mManager.registerAttribute<glm::vec3>("acceleration");
    mManager.registerAttribute<float>("pitch");
    mManager.registerAttribute<float>("yaw");
    mManager.registerAttribute<uint32_t>("physics_type");
    mManager.registerAttribute<float>("drag");
    mManager.registerAttribute<glm::vec3>("hitbox");
    mManager.registerAttribute<bool>("on_ground");
    mManager.registerAttribute<ChunkCoord>("current_chunk");
    mManager.registerAttribute<VoxelCoord>("block_facing");
    mManager.registerAttribute<bool>("is_facing_block");
    mManager.registerAttribute<uint32_t>("block_facing_face");
    mManager.registerAttribute<float>("walk_speed");
    mManager.registerAttribute<float>("run_speed");
    mManager.registerAttribute<float>("jump_strength");
    mManager.registerAttribute<bool>("jumping");
    mManager.registerAttribute<uint32_t>("move_action");
    mManager.registerAttribute<int16_t>("move_direction");
    mFactory.registerEntityTemplates(loader.loadEntityTemplates("data/entities.json"));
    mFactory.registerDefaultSetter("position", vec3Setter);
    mFactory.registerDefaultSetter("velocity", vec3Setter);
    mFactory.registerDefaultSetter("acceleration", vec3Setter);
    mFactory.registerDefaultSetter("pitch", fea::util::floatSetter);
    mFactory.registerDefaultSetter("yaw", fea::util::floatSetter);
    mFactory.registerDefaultSetter("hitbox", vec3Setter);
    mFactory.registerDefaultSetter("on_ground", fea::util::boolSetter);
    mFactory.registerDefaultSetter("jumping", fea::util::boolSetter);
}

fea::WeakEntityPtr EntityFactory::spawnEntity(const std::string& scriptType)
{
    if(mEntityDefinitions.find(scriptType) == mEntityDefinitions.end())
    {
        std::cout << "Error! Trying to spawn entity of type " << scriptType << " but it lacks a definition file\n";
        return fea::WeakEntityPtr();
    }

    const EntityDefinition& definition = mEntityDefinitions.at(scriptType);
    std::string category = definition.category;

    fea::EntityPtr spawned = mFactory.createEntity(category).lock();
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
