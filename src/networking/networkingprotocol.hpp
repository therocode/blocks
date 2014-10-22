#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <fea/entitysystem.hpp>
#include "../world/chunk.hpp"
#include "../world/worlddefines.hpp"
#include "../input/inputactions.hpp"
#include "../entity/controllers/movedirection.hpp"
#include "../entity/controllers/moveaction.hpp"
#include <cereal/types/vector.hpp>
#include <cereal/types/array.hpp>
#include "../utilities/namedenum.hpp"

named_enum(PacketType, INVALID, 
    //joining
    CLIENT_JOIN_REQUESTED, CLIENT_JOIN_DENIED, CLIENT_JOIN_ACCEPTED, SUBSCRIPTION_REQUESTED, SUBSCRIPTION_REPLY,
    //chunks
    CLIENT_REQUESTED_CHUNKS, CLIENT_CHUNKS_DENIED, CLIENT_CHUNKS_DELIVERY, VOXEL_UPDATED,
    //controls
    CLIENT_ACTION, CLIENT_MOVE_ACTION, CLIENT_MOVE_DIRECTION, CLIENT_PITCH_YAW,
    //entities
    CLIENT_ATTACHED_TO_ENTITY, ENTITY_ENTERED_RANGE, ENTITY_POSITION_UPDATED, ENTITY_ROTATION_UPDATED, ENTITY_LEFT_RANGE,
    //client
    CLIENT_ENTERED_WORLD,
    CLIENT_POSITION,
    //tests
    TEST_1, TEST_2);

//network protocol:
//joining
struct ClientJoinRequestedMessage
{
    PacketType getType() const {return CLIENT_JOIN_REQUESTED;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(playerName);
    }

    std::string playerName;
};

enum JoinDenyReason { FULL };

struct ClientJoinDeniedMessage
{
    PacketType getType() const {return CLIENT_JOIN_DENIED;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(reason, playerAmount, maximumAllowed);
    }

    JoinDenyReason reason;
    uint32_t playerAmount;   
    uint32_t maximumAllowed;   
};

struct ServerNetSettings
{
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(serverName, motd, maxChunkViewDistance);
    }

    std::string serverName;
    std::string motd;
    int32_t maxChunkViewDistance;
};

struct ClientJoinAcceptedMessage
{
    PacketType getType() const {return CLIENT_JOIN_ACCEPTED;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(settings);
    }

    ServerNetSettings settings;
};

//chunks
using ChunkCoord = glm::i64vec3;

namespace glm
{
    namespace detail
    {
        template<class Archive>
            void serialize(Archive& archive, ChunkCoord& vec)
            {
                archive(vec.x, vec.y, vec.z);
            }
    }
}

struct ClientRequestedChunksMessage
{
    PacketType getType() const {return CLIENT_REQUESTED_CHUNKS;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(worldIdentifier, coordinates);
    }

    std::string worldIdentifier;
    std::vector<ChunkCoord> coordinates;
};

struct ClientChunksDeniedMessage
{
    PacketType getType() const {return CLIENT_CHUNKS_DENIED;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(worldIdentifier, coordinates);
    }

    std::string worldIdentifier;
    std::vector<ChunkCoord> coordinates;
};

struct ClientChunksDeliveredMessage
{
    PacketType getType() const {return CLIENT_CHUNKS_DELIVERY;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(worldIdentifier, coordinates, rleIndices, rleSegments);
    }

    std::string worldIdentifier;
    std::vector<ChunkCoord> coordinates;
    std::vector<RleIndexArray> rleIndices;
    std::vector<RleSegmentArray> rleSegments;
};

struct VoxelUpdatedMessage
{
    PacketType getType() const {return VOXEL_UPDATED;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(voxel.x, voxel.y, voxel.z, type);
    }

    VoxelCoord voxel;
    VoxelType type;
};

struct ClientActionMessage
{
    PacketType getType() const {return CLIENT_ACTION;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(action);
    }

    InputAction action;
};

struct ClientMoveDirectionMessage
{
    PacketType getType() const {return CLIENT_MOVE_DIRECTION;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(direction);
    }

    MoveDirection direction;
};

struct ClientMoveActionMessage
{
    PacketType getType() const {return CLIENT_MOVE_ACTION;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(action);
    }

    MoveAction action;
};

struct ClientPitchYawMessage
{
    PacketType getType() const {return CLIENT_PITCH_YAW;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(pitch, yaw);
    }

    float pitch, yaw;
};

struct ClientAttachedToEntityMessage
{
    PacketType getType() const {return CLIENT_ATTACHED_TO_ENTITY;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(entityId, world, position.x, position.y, position.z, highlightRange);
    }

    fea::EntityId entityId;
    std::string world;
    glm::vec3 position;
    uint32_t highlightRange;
};

struct SubscriptionRequestedMessage
{
    PacketType getType() const {return SUBSCRIPTION_REQUESTED;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(chunkDistance);
    }

    uint32_t chunkDistance;
};

struct SubscriptionReplyMessage
{
    PacketType getType() const {return SUBSCRIPTION_REPLY;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(granted);
    }

    bool granted;
};

struct EntityEnteredRangeMessage
{
    PacketType getType() const {return ENTITY_ENTERED_RANGE;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(id, position.x, position.y, position.z);
    }

    fea::EntityId id;
    //type
    glm::vec3 position;
    //float pitch;
    //float yaw; add these later
};

struct EntityPositionUpdatedMessage
{
    PacketType getType() const {return ENTITY_POSITION_UPDATED;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(id, position.x, position.y, position.z);
    }

    fea::EntityId id;
    //type
    glm::vec3 position;
    //float pitch;
    //float yaw; add these later
};

struct EntityRotationUpdatedMessage
{
    PacketType getType() const {return ENTITY_ROTATION_UPDATED;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(id, pitch, yaw);
    }

    fea::EntityId id;
    //type
    float pitch;
    float yaw;
};

struct EntityLeftRangeMessage
{
    PacketType getType() const {return ENTITY_LEFT_RANGE;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(id);
    }

    fea::EntityId id;
};

struct ClientEnteredWorldMessage
{
    PacketType getType() const {return CLIENT_ENTERED_WORLD;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(world);
    }

    std::string world;
};

struct ClientPositionMessage
{
    PacketType getType() const {return CLIENT_POSITION;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(position.x, position.y, position.z);
    }

    glm::vec3 position;
};

//messages for testing:
struct TestMessage1
{
    PacketType getType() const
    {
        return TEST_1;
    }

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(text, numerical);
    }

    std::string text;
    int32_t numerical;
};

struct TestMessage2
{
    PacketType getType() const
    {
        return TEST_2;
    }

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(textNumbersMap, floatNumerical);
    }

    std::unordered_map<std::string, std::vector<int32_t>> textNumbersMap;
    double floatNumerical;
};
