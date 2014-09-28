#pragma once
#include "blockstd.hpp"
#include "scriptinterface.hpp"
#include "utilities/random.hpp"
#include "world/worldid.hpp"

class GameInterface;
using VoxelType = uint16_t;

class LandscapeInterface : public ScriptInterface
{
    public:
        LandscapeInterface(fea::MessageBus& bus, GameInterface& worldInterface);
        void registerInterface(asIScriptEngine* engine) override;
    private:
        void setVoxelType(WorldId id, const glm::vec3& coordinate, uint16_t type);
        VoxelType getVoxelType(WorldId id, const glm::vec3& coordinate);
        Random random;
};
