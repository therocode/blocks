#pragma once
#include "world.h"
#include "../entity/entitysystem.h"
#include "chunk.h"

class asIScriptObject;

class WorldInterface
{
    public:
        WorldInterface(World& dimension, EntitySystem& entitySystem);
		VoxelType getVoxelTypeInt(int x, int y, int z) const;
        VoxelType getVoxelType(float x, float y, float z) const; 
        VoxelType getVoxelType(const glm::vec3& position) const; 
        VoxelType getVoxelType(const VoxelWorldCoordinate coord) const;
		bool getVoxelAtRay(const glm::vec3& position, const glm::vec3& direction, const float maxDistance, int& hitFace, VoxelWorldCoordinate& hitBlock ) const;
		bool getVoxelAtRay(float ox, float oy, float oz, float dx, float dy, float dz, const float maxDistance, int& hitFace, VoxelWorldCoordinate& hitBlock) const;
		
        fea::WeakEntityPtr createEntity(const std::string& scriptType, const glm::vec3& position);
        ChunkReferenceMap getChunkMap() const;

        template<class Type>
        Type getEntityAttribute(fea::EntityId id, const std::string& name)
        {
            return mEntitySystem.getEntityAttribute<Type>(id, name);
        }
        EntityCreator getEntityCreator() const;
    private:
        World& mWorld;
        EntitySystem& mEntitySystem;
};
