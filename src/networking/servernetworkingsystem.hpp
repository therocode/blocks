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
#include "enet.hpp"
#include "enetserver.hpp"

using ClientId = size_t;

class ServerNetworkingSystem : public fea::MessageReceiver<
                                           LocalConnectionAttemptMessage,
                                           FrameMessage,
                                           GameStartMessage>
{
    public:
        ServerNetworkingSystem(fea::MessageBus& bus, const NetworkParameters& parameters);
        void handleMessage(const LocalConnectionAttemptMessage& received);
        void handleMessage(const FrameMessage& received);
        void handleMessage(const GameStartMessage& received);
    private:
        void acceptRemoteClient(uint32_t id);
        void handleClientData(uint32_t clientId, const std::vector<uint8_t>& data);
        void disconnectRemoteClient(uint32_t id);

        fea::MessageBus& mBus;
        NetworkParameters mParameters;
        bool mAcceptingClients;

        std::unordered_set<size_t> graphicsEntities; //temporary solution on how to resend things

        uint32_t mNextClientId;
        std::unordered_map<uint32_t, uint32_t> mClientToPlayerIds;
        std::unordered_map<uint32_t, uint32_t> mPlayerToClientIds;

        fea::MessageBus* mLocalClientBus;
        uint32_t mLocalPlayerId;

        std::unique_ptr<ENet> mENet;
        std::unique_ptr<ENetServer> mENetServer;
};
