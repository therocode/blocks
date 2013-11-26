#pragma once
#include "blockstd.h"
#include "scriptinterface.h"
#include "utilities/random.h"
#include "world/chunk.h"

class LandscapeInterface : public ScriptInterface
{
    public:
        LandscapeInterface(fea::MessageBus& bus, WorldInterface& worldInterface);
        void registerInterface(asIScriptEngine* engine) override;
    private:
        void setVoxelType(float x, float y, float z, uint16_t type);
        void setVoxelType(const glm::vec3& coordinate, uint16_t type);
        VoxelType getVoxelType(float x, float y, float z);
        VoxelType getVoxelType(const glm::vec3& coordinate);
        Random random;
};
