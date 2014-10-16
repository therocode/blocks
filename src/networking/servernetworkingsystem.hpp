#pragma once
#include <fea/util.hpp>
#include <memory>
#include <vector>
#include <unordered_map>
#include "networkparameters.hpp"
#include "../rendering/renderingmessages.hpp"
#include "../entity/entitymessages.hpp"
#include "../world/worldmessages.hpp"
#include "../application/applicationmessages.hpp"
#include "networkingmessages.hpp"
#include "networkingprotocol.hpp"
#include "enet.hpp"
#include "enetserver.hpp"
#include "../utilities/glmhash.hpp"
#include "chunkrequesthandler.hpp"

using ClientId = size_t;
class GameInterface;

class ServerNetworkingSystem : public fea::MessageReceiver<
                                           LocalConnectionAttemptMessage,
                                           LocalDisconnectionMessage,
                                           FrameMessage,
                                           GameStartMessage,
                                           ClientJoinRequestedMessage,
                                           ClientRequestedChunksMessage,
                                           ClientActionMessage,
                                           ClientMoveActionMessage,
                                           ClientMoveDirectionMessage,
                                           PlayerEntersChunkMessage,
                                           PlayerEntersWorldMessage,
                                           ChunksDataDeniedMessage,
                                           ChunksDataDeliveredMessage,
                                           ChunkFinishedMessage>
{
    public:
        ServerNetworkingSystem(fea::MessageBus& bus, const GameInterface& gameInterface, const NetworkParameters& parameters);
        void handleMessage(const LocalConnectionAttemptMessage& received);
        void handleMessage(const LocalDisconnectionMessage& received);
        void handleMessage(const FrameMessage& received);
        void handleMessage(const GameStartMessage& received);
        void handleMessage(const ClientJoinRequestedMessage& received);
        void handleMessage(const ClientRequestedChunksMessage& received);
        void handleMessage(const ClientActionMessage& received);
        void handleMessage(const ClientMoveActionMessage& received);
        void handleMessage(const ClientMoveDirectionMessage& received);
        void handleMessage(const PlayerEntersChunkMessage& received);
        void handleMessage(const PlayerEntersWorldMessage& received);
        void handleMessage(const ChunksDataDeniedMessage& received);
        void handleMessage(const ChunksDataDeliveredMessage& received);
        void handleMessage(const ChunkFinishedMessage& received);
    private:
        void acceptRemoteClient(uint32_t id);
        void handleClientData(uint32_t clientId, const std::vector<uint8_t>& data);
        void disconnectRemoteClient(uint32_t id);
        void playerRequestedChunks(uint32_t id, const std::string& worldIdentifier, const std::vector<ChunkCoord>& chunks);
        template <typename Message>
        void sendToOne(uint32_t playerId, const Message& message, bool reliable, uint8_t channel);
        template <typename Message>
        void sendToAll(const Message& message, bool reliable, uint8_t channel);

        fea::MessageBus& mBus;
        const GameInterface& mGameInterface;
        NetworkParameters mParameters;
        bool mAcceptingClients;
        uint32_t mPlayerAmount;
        uint32_t mMaxPlayerAmount;
        ServerNetSettings mSettings;

        uint32_t mNextClientId;
        std::unordered_map<uint32_t, uint32_t> mClientToPlayerIds;
        std::unordered_map<uint32_t, uint32_t> mPlayerToClientIds;

        std::unordered_map<uint32_t, ChunkCoord> mPlayerPositions;
        std::unordered_map<uint32_t, WorldId> mPlayerWorlds;

        std::unordered_map<WorldId, ChunkRequestHandler> mChunkRequestHandlers;

        fea::MessageBus* mLocalClientBus;
        uint32_t mLocalPlayerId;

        std::unique_ptr<ENet> mENet;
        std::unique_ptr<ENetServer> mENetServer;
};

template <typename Message>
void ServerNetworkingSystem::sendToOne(uint32_t playerId, const Message& message, bool reliable, uint8_t channel)
{
    if(mLocalPlayerId == playerId)
    {
        FEA_ASSERT(mLocalClientBus != nullptr, "local player ID exists, but bus does not. This is bad.");
        mLocalClientBus->send(message);
    }
    else
    {
        FEA_ASSERT(mPlayerToClientIds.count(playerId) != 0, "Trying to send message to invalid player ID " + std::to_string(playerId));
        mENetServer->sendToOne(mPlayerToClientIds.at(playerId), serializeMessage(message), reliable, channel);
    }
}

template <typename Message>
void ServerNetworkingSystem::sendToAll(const Message& message, bool reliable, uint8_t channel)
{
    if(mLocalClientBus)
        mLocalClientBus->send(message);
    else
        mENetServer->sendToAll(serializeMessage(message), reliable, channel);
}
