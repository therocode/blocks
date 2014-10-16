#pragma once
#include "../networking/networkparameters.hpp"
#include "../networking/networkingmessages.hpp"
#include "../networking/messageserializer.hpp"
#include "../application/applicationmessages.hpp"
#include "../entity/entitymessages.hpp"
#include "../rendering/renderingmessages.hpp"
#include "../script/scriptmessages.hpp"
#include "../input/inputmessages.hpp"
#include "enet.hpp"
#include "enetclient.hpp"
#include "networkingprotocol.hpp"

class ENet;
class ENetClient;

class ClientNetworkingSystem : public
    fea::MessageReceiver<FrameMessage,
                         LocalConnectionEstablishedMessage,
                         ClientJoinDeniedMessage,
                         ClientJoinAcceptedMessage,
                         ClientRequestedChunksMessage,
                         ClientChunksDeniedMessage,
                         EntitySubscriptionReplyMessage,
                         ClientActionMessage,
                         ClientMoveActionMessage,
                         ClientMoveDirectionMessage,
                         ClientPitchYawMessage,
                         EntityEnteredRangeMessage,
                         EntityPositionUpdatedMessage>
{
    public:
        ClientNetworkingSystem(fea::MessageBus& bus, const NetworkParameters& parameters);
        ~ClientNetworkingSystem();
        void handleMessage(const FrameMessage& received) override;
        void handleMessage(const LocalConnectionEstablishedMessage& received) override;
        void handleMessage(const ClientJoinDeniedMessage& received) override;
        void handleMessage(const ClientJoinAcceptedMessage& received) override;
        void handleMessage(const ClientRequestedChunksMessage& received) override;
        void handleMessage(const ClientChunksDeniedMessage& received) override;
        void handleMessage(const EntitySubscriptionReplyMessage& received) override;
        void handleMessage(const ClientActionMessage& received) override;
        void handleMessage(const ClientMoveActionMessage& received) override;
        void handleMessage(const ClientMoveDirectionMessage& received) override;
        void handleMessage(const ClientPitchYawMessage& received) override;
        void handleMessage(const EntityEnteredRangeMessage& received) override;
        void handleMessage(const EntityPositionUpdatedMessage& received) override;
    private:
        void connectedToServer();
        void handleServerData(const std::vector<uint8_t>& data);
        void disconnectedFromServer();
        template <typename Message>
        void send(const Message& message, bool reliable, uint8_t channel);
        fea::MessageBus& mBus;

        NetworkParameters mParameters;
        fea::MessageBus* mServerBus;
        std::unique_ptr<ENet> mENet;
        std::unique_ptr<ENetClient> mENetClient;
};

template <typename Message>
void ClientNetworkingSystem::send(const Message& message, bool reliable, uint8_t channel)
{
    if(mServerBus)
        mServerBus->send(message);
    else
        mENetClient->send(serializeMessage(message), reliable, channel);
}
