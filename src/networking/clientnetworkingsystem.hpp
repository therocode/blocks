#pragma once
#include "../networking/networkparameters.hpp"
#include "../networking/networkingmessages.hpp"
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
                         PlayerActionMessage,
                         PlayerMoveDirectionMessage,
                         PlayerMoveActionMessage,
                         PlayerPitchYawMessage,
                         RebuildScriptsRequestedMessage,
                         LocalConnectionEstablishedMessage>
{
    public:
        ClientNetworkingSystem(fea::MessageBus& bus, const NetworkParameters& parameters);
        void handleMessage(const FrameMessage& received) override;
        void handleMessage(const PlayerActionMessage& received) override;
        void handleMessage(const PlayerMoveDirectionMessage& received) override;
        void handleMessage(const PlayerMoveActionMessage& received) override;
        void handleMessage(const PlayerPitchYawMessage& received) override;
        void handleMessage(const RebuildScriptsRequestedMessage& received) override;
        void handleMessage(const LocalConnectionEstablishedMessage& received) override;
    private:
        void connectedToServer();
        void handleServerData(const std::vector<uint8_t>& data);
        void disconnectedFromServer();
        fea::MessageBus& mBus;

        NetworkParameters mParameters;
        fea::MessageBus* mServerBus;
        std::unique_ptr<ENet> mENet;
        std::unique_ptr<ENetClient> mENetClient;
};
