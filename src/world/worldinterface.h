#pragma once
#include "dimension.h"
#include "../entity/entitysystem.h"

class asIScriptObject;

class WorldInterface
{
    public:
        WorldInterface(Dimension& dimension, EntitySystem& entitySystem);
        VoxelType getVoxelType(float x, float y, float z) const; 
        VoxelType getVoxelType(const glm::vec3& position) const; 
		glm::vec3 getVoxelAtRay(const glm::vec3& position, const glm::vec3& direction) const;
		glm::vec3 getVoxelAtRay(float ox, float oy, float oz, float dx, float dy, float dz) const;
		
        fea::WeakEntityPtr spawnEntity(const std::string& scriptType, const glm::vec3& position);
        size_t spawnEntityFromScriptHandle(const std::string& scriptType, const glm::vec3& position, asIScriptObject* obj);
        const ChunkMap& getChunkList() const;

        template<class Type>
        Type getEntityAttribute(fea::EntityId id, const std::string& name)
        {
            return mEntitySystem.getEntityAttribute<Type>(id, name);
        }
    private:
        Dimension& mDimension;
        EntitySystem& mEntitySystem;
};
