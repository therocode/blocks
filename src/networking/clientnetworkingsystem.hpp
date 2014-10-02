#pragma once
#include "../networking/networkparameters.hpp"
#include "../application/applicationmessages.hpp"
#include "../entity/entitymessages.hpp"
#include "../rendering/renderingmessages.hpp"
#include "../script/scriptmessages.hpp"
#include "../input/inputmessages.hpp"

class ENet;
class ENetClient;

class ClientNetworkingSystem : public
    fea::MessageReceiver<FrameMessage,
                         PlayerActionMessage,
                         PlayerMoveDirectionMessage,
                         PlayerMoveActionMessage,
                         PlayerPitchYawMessage,
                         RebuildScriptsRequestedMessage>
{
    public:
        ClientNetworkingSystem(fea::MessageBus& bus, const NetworkParameters& parameters);
        void handleMessage(const FrameMessage& received) override;
        void handleMessage(const PlayerActionMessage& received) override;
        void handleMessage(const PlayerMoveDirectionMessage& received) override;
        void handleMessage(const PlayerMoveActionMessage& received) override;
        void handleMessage(const PlayerPitchYawMessage& received) override;
        void handleMessage(const RebuildScriptsRequestedMessage& received) override;
    private:
        void fetchServerData();
        fea::MessageBus& mBus;

        NetworkParameters mParameters;
        std::unique_ptr<ENet> mEnet;
        std::unique_ptr<ENetClient> mEnetClient;
};
