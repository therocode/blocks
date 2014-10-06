#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>

enum { INVALID = -1, CLIENT_JOIN_REQUESTED, CLIENT_JOIN_DENIED, CLIENT_JOIN_ACCEPTED, TEST_1, TEST_2 };

//network protocol:
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
