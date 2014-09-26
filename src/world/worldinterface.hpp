#pragma once
#include "../entity/entitysystem.hpp"
#include "chunk.hpp"
#include <memory>

class asIScriptObject;
class World;

class WorldInterface
{
    public:
        WorldInterface(std::unordered_map<WorldId, std::unique_ptr<World>>& worlds, EntitySystem& entitySystem);
		VoxelType getVoxelTypeInt(WorldId worldId, int x, int y, int z) const;
        VoxelType getVoxelType(WorldId worldId, float x, float y, float z) const; 
        VoxelType getVoxelType(WorldId worldId, const glm::vec3& position) const; 
        VoxelType getVoxelType(WorldId worldId, const VoxelCoord coord) const;
		bool getVoxelAtRay(WorldId worldId, const glm::vec3& position, const glm::vec3& direction, const float maxDistance, uint32_t& hitFace, VoxelCoord& hitBlock ) const;
		bool getVoxelAtRay(WorldId worldId, float ox, float oy, float oz, float dx, float dy, float dz, const float maxDistance, uint32_t& hitFace, VoxelCoord& hitBlock) const;
		
        ChunkReferenceMap getChunkMap(WorldId worldId) const;

        const fea::WeakEntityPtr findEntity(fea::EntityId id) const;
    private:
        std::unordered_map<WorldId, std::unique_ptr<World>>& mWorlds;
        EntitySystem& mEntitySystem;
};
