#pragma once
#include "../world/worlddefines.hpp"

struct ClientChunkDeletedMessage
{
    const ChunkCoord& coordinate;
};

struct LocalPlayerAttachedToEntityMessage
{
    fea::EntityId entityId;
    WorldId worldId;
    const glm::vec3& position;
    uint32_t highlightRange;
};

struct ClientEntityMovedMessage
{
    fea::EntityId id;
    const glm::vec3& position;
};
