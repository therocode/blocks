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
		
        const WorldSystem& getWorldSystem() const;
        const EntitySystem& getEntitySystem() const;
    private:
        const WorldSystem&  mWorldSystem;
        const EntitySystem& mEntitySystem;
};
