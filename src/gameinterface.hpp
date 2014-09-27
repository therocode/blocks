#pragma once
#include "world/chunk.hpp"
#include <fea/entitysystem.hpp>
#include <memory>
#include <unordered_map>

class WorldSystem;
class EntitySystem;

class GameInterface
{
    public:
        GameInterface(const WorldSystem& worldSystem, const EntitySystem& entitySystem);
		bool getVoxelAtRay(WorldId worldId, const glm::vec3& position, const glm::vec3& direction, const float maxDistance, uint32_t& hitFace, VoxelCoord& hitBlock ) const;
		
        const WorldSystem& getWorldSystem() const;
        const EntitySystem& getEntitySystem() const;
    private:
        const WorldSystem&  mWorldSystem;
        const EntitySystem& mEntitySystem;
};
