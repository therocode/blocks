#include "gameinterface.hpp"
#include "rendering/renderer.hpp"
#include "entity/entitysystem.hpp"
#include "world/worldsystem.hpp"

GameInterface::GameInterface(const WorldSystem& worldSystem, const EntitySystem& entitySystem) :
    mWorldSystem(worldSystem),
    mEntitySystem(entitySystem)
{

}

const WorldSystem& GameInterface::getWorldSystem() const
{
    return mWorldSystem;
}

const EntitySystem& GameInterface::getEntitySystem() const
{
    return mEntitySystem;
}
