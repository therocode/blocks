#pragma once
#include "world.hpp"
#include "../entity/entitysystem.hpp"
#include "chunk.hpp"

class asIScriptObject;

class WorldInterface
{
    public:
        WorldInterface(std::unordered_map<std::string, World>& worlds, EntitySystem& entitySystem);
		VoxelType getVoxelTypeInt(int x, int y, int z) const;
        VoxelType getVoxelType(float x, float y, float z) const; 
        VoxelType getVoxelType(const glm::vec3& position) const; 
        VoxelType getVoxelType(const VoxelCoord coord) const;
		bool getVoxelAtRay(const glm::vec3& position, const glm::vec3& direction, const float maxDistance, uint32_t& hitFace, VoxelCoord& hitBlock ) const;
		bool getVoxelAtRay(float ox, float oy, float oz, float dx, float dy, float dz, const float maxDistance, uint32_t& hitFace, VoxelCoord& hitBlock) const;
		
        fea::WeakEntityPtr createEntity(const std::string& scriptType, const glm::vec3& position);
        ChunkReferenceMap getChunkMap() const;

        template<class Type>
        Type getEntityAttribute(fea::EntityId id, const std::string& name)
        {
            return mEntitySystem.getEntityAttribute<Type>(id, name);
        }
        EntityCreator getEntityCreator() const;

        void addHighlightEntity(const std::string& world, fea::EntityId id, const ChunkCoord& coordinate);
        void removeHighlightEntity(const std::string& world, fea::EntityId id);
        void moveHighlightEntity(const std::string& world, fea::EntityId id, const ChunkCoord& coordinate);
    private:
        std::unordered_map<std::string, World>& mWorlds;
        EntitySystem& mEntitySystem;
};
