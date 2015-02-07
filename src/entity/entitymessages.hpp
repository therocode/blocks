#pragma once
#include "../utilities/glm.hpp"
#include "controllers/movedirection.hpp"
#include "controllers/moveaction.hpp"
#include <fea/util.hpp>
#include <fea/entitysystem.hpp>
#include "../input/inputactions.hpp"
#include "../world/worlddefines.hpp"

struct EntityMoveRequestedMessage
{
    fea::EntityId id; const glm::vec3& newPosition;
};

struct EntityMovedMessage
{
    fea::EntityId entityId; WorldId worldId; const glm::vec3& oldPosition; const glm::vec3& newPosition;
};

struct EntityWorldTransferRequestedMessage
{
    fea::EntityPtr entity; WorldId worldId;
};

struct EntityTransferringWorldMessage
{
    fea::EntityId entityId; WorldId oldWorldId; WorldId newWorldId;
};

struct EntityTransferredWorldMessage
{
    fea::EntityId entityId; WorldId oldWorld; WorldId newWorld;
};

struct EntityOrientedMessage
{
    fea::EntityId id; const glm::quat &orientation;
};

struct GravityRequestedMessage
{
    float gravityConstant;
};

struct EntityRequestedMessage
{
    const std::string& type; std::function<void(fea::EntityPtr)> initializer;
};

struct EntityCreatedMessage
{
    fea::WeakEntityPtr entity; const std::string& type;
};

struct EntityJumpMessage
{
    fea::EntityId id; const bool jumping;
};

struct EntityOnGroundMessage
{
    fea::EntityId id; bool landed;
};

struct RemoveEntityRequestedMessage
{
    fea::EntityId id;
};

struct EntityRemovedMessage
{
    fea::EntityId id;
};

struct PhysicsImpulseMessage
{
    fea::EntityId id; const glm::vec3& force;
};
