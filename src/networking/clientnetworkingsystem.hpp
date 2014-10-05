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

class ENet;
class ENetClient;

class ClientNetworkingSystem : public
    fea::MessageReceiver<FrameMessage,
                         LocalConnectionEstablishedMessage>
{
    public:
        ClientNetworkingSystem(fea::MessageBus& bus, const NetworkParameters& parameters);
        void handleMessage(const FrameMessage& received) override;
        void handleMessage(const LocalConnectionEstablishedMessage& received) override;
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
        mBus.send(message);
    else
        mENetClient->send(serializeMessage(message), reliable, channel);
}
