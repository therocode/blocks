#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>
#include "../world/chunk.hpp"
#include "../world/worlddefines.hpp"
#include "../input/inputactions.hpp"
#include "../entity/controllers/movedirection.hpp"
#include "../entity/controllers/moveaction.hpp"
#include <cereal/types/vector.hpp>
#include <cereal/types/array.hpp>

enum { INVALID = -1, 
    //joining
    CLIENT_JOIN_REQUESTED, CLIENT_JOIN_DENIED, CLIENT_JOIN_ACCEPTED,
    //chunks
    CLIENT_REQUESTED_CHUNKS, CLIENT_CHUNKS_DENIED, CLIENT_CHUNKS_DELIVERY,
    //controls
    CLIENT_ACTION, CLIENT_MOVE_ACTION, CLIENT_MOVE_DIRECTION,
    //tests
    TEST_1, TEST_2 };

//network protocol:
//joining
struct ClientJoinRequestedMessage
{
    int32_t getType() const {return CLIENT_JOIN_REQUESTED;}

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
    int32_t getType() const {return CLIENT_JOIN_DENIED;}

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
    int32_t getType() const {return CLIENT_JOIN_ACCEPTED;}

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
    int32_t getType() const {return CLIENT_REQUESTED_CHUNKS;}

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
    int32_t getType() const {return CLIENT_CHUNKS_DENIED;}

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
    int32_t getType() const {return CLIENT_CHUNKS_DELIVERY;}

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

struct ClientActionMessage
{
    int32_t getType() const {return CLIENT_ACTION;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(action);
    }

    InputAction action;
};

struct ClientMoveDirectionMessage
{
    int32_t getType() const {return CLIENT_MOVE_DIRECTION;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(direction);
    }

    MoveDirection direction;
};

struct ClientMoveActionMessage
{
    int32_t getType() const {return CLIENT_MOVE_ACTION;}

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(action);
    }

    MoveAction action;
};

//messages for testing:
struct TestMessage1
{
    int32_t getType() const
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
    int32_t getType() const
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
