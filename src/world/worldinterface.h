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
        const std::vector<Chunk>& getChunkList() const;
    private:
        Dimension& mDimension;
        EntitySystem& mEntitySystem;
};
