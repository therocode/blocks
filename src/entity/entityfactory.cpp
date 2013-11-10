#include <featherkit/entitysystemutil.h>
#include <iostream>
#include "../blockstd.h"
#include "entityfactory.h"
#include "defaultsetters.h"
#include "../world/chunk.h"
#include "controllers/moveaction.h"
#include "controllers/movedirection.h"

EntityFactory::EntityFactory(fea::EntityManager& manager) : mManager(manager)
{
    //register templates
    fea::util::JsonEntityLoader loader;
    loader.registerType("#vec3#",sizeof(glm::vec3));
    loader.registerType("#int16#",sizeof(int16_t));
    loader.registerType("#chunkcoordinate#",sizeof(ChunkCoordinate));
	loader.registerType("#voxelworldcoordinate#", sizeof(VoxelWorldCoordinate));
    mManager.registerAttributes(loader.loadAttributesJson("data/attributes.json"));
    mManager.registerEntityTemplates(loader.loadEntitiesJson("data/entities.json"));
    mManager.registerDefaultSetter("position", vec3Setter);
    mManager.registerDefaultSetter("velocity", vec3Setter);
    mManager.registerDefaultSetter("acceleration", vec3Setter);
    mManager.registerDefaultSetter("pitch", fea::util::floatSetter);
    mManager.registerDefaultSetter("yaw", fea::util::floatSetter);
    mManager.registerDefaultSetter("hitbox", vec3Setter);
    mManager.registerDefaultSetter("on_ground", fea::util::boolSetter);
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

    fea::EntityPtr spawned = mManager.createEntity(category).lock();
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
