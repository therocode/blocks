#pragma once
#include "../networking/networkparameters.hpp"
#include "../networking/networkingmessages.hpp"
#include "../networking/messageserializer.hpp"
#include "../application/applicationmessages.hpp"
#include "../entity/entitymessages.hpp"
#include "../rendering/renderingmessages.hpp"
#include "../script/scriptmessages.hpp"
#include "../input/inputmessages.hpp"
#include "../utilities/idmapper.hpp"
#include "../world/worldmessages.hpp"
#include "../resources/resourcesystem.hpp"
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
                         ChunksRequestedMessage,
                         ClientChunksDeliveredMessage,
                         ClientChunksDeniedMessage,
                         SubscriptionReplyMessage,
                         ClientActionMessage,
                         ClientMoveActionMessage,
                         ClientMoveDirectionMessage,
                         ClientPitchYawMessage,
                         EntityEnteredRangeMessage,
                         EntityPositionUpdatedMessage,
                         EntityOrientationUpdatedMessage,
                         EntityLeftRangeMessage,
                         EntityAnimationMessage,
                         ClientAttachedToEntityMessage,
                         VoxelUpdatedMessage,
                         SelectVoxelDeltaMessage>
{
    public:
        ClientNetworkingSystem(fea::MessageBus& bus, const NetworkParameters& parameters, const ResourceSystem& resources);
        ~ClientNetworkingSystem();
        void handleMessage(const FrameMessage& received) override;
        void handleMessage(const LocalConnectionEstablishedMessage& received) override;
        void handleMessage(const ClientJoinDeniedMessage& received) override;
        void handleMessage(const ClientJoinAcceptedMessage& received) override;
        void handleMessage(const ChunksRequestedMessage& received) override;
        void handleMessage(const ClientChunksDeliveredMessage& received) override;
        void handleMessage(const ClientChunksDeniedMessage& received) override;
        void handleMessage(const SubscriptionReplyMessage& received) override;
        void handleMessage(const ClientActionMessage& received) override;
        void handleMessage(const ClientMoveActionMessage& received) override;
        void handleMessage(const ClientMoveDirectionMessage& received) override;
        void handleMessage(const ClientPitchYawMessage& received) override;
        void handleMessage(const EntityEnteredRangeMessage& received) override;
        void handleMessage(const EntityPositionUpdatedMessage& received) override;
        void handleMessage(const EntityOrientationUpdatedMessage& received) override;
        void handleMessage(const EntityLeftRangeMessage& received) override;
        void handleMessage(const EntityAnimationMessage& received) override;
        void handleMessage(const ClientAttachedToEntityMessage& received) override;
        void handleMessage(const VoxelUpdatedMessage& received) override;
        void handleMessage(const SelectVoxelDeltaMessage& received) override;
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
        bool mIsConnected;
        IdMapper<std::string, WorldId> mWorldIds;
        size_t mPlayerEntity; //hack to remove when server/client traffic is cleaned up
        std::unordered_map<size_t, size_t> mGraphicEntityIds;
        std::unordered_map<WorldId, std::vector<ChunkCoord>> mChunksToRequest;
        const ResourceSystem& mResources; //should this one be here?
};

template <typename Message>
void ClientNetworkingSystem::send(const Message& message, bool reliable, uint8_t channel)
{
    if(mServerBus)
    {
        mServerBus->send(message);
    }
    else
        mENetClient->send(serializeMessage(message), reliable, channel);
}
