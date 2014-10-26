#pragma once
#include "scriptinterface.hpp"
#include "../../utilities/random.hpp"
#include "../../utilities/glm.hpp"
#include "../../world/worlddefines.hpp"

class GameInterface;
using VoxelType = uint16_t;

class WorldInterface : public ScriptInterface
{
    public:
        WorldInterface(fea::MessageBus& bus, GameInterface& worldInterface);
        void registerInterface(asIScriptEngine* engine) override;
    private:
        void setVoxelType(WorldId id, const glm::vec3& coordinate, uint16_t type);
        VoxelType getVoxelType(WorldId id, const glm::vec3& coordinate);
        Random random;
};
