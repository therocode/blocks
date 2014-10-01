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

class ServerNetworkingSystem : public fea::MessageReceiver<LocalConnectionAttemptMessage,
                                           FrameMessage,
                                           IncomingConnectionMessage,
                                           ClientPackageReceived,
                                           ClientDisconnectedMessage,
                                           AddGfxEntityMessage,
                                           MoveGfxEntityMessage,
                                           RotateGfxEntityMessage,
                                           RemoveGfxEntityMessage,
                                           PlayerConnectedToEntityMessage,
                                           PlayerFacingBlockMessage,
                                           ChunkLoadedMessage,
                                           ChunkDeletedMessage,
                                           VoxelSetMessage>
{
    public:
        ServerNetworkingSystem(fea::MessageBus& bus, const NetworkParameters& parameters);
        void handleMessage(const LocalConnectionAttemptMessage& received);
        void handleMessage(const FrameMessage& received);
        void handleMessage(const IncomingConnectionMessage& received);
        void handleMessage(const ClientPackageReceived& received);
        void handleMessage(const ClientDisconnectedMessage& received);
        void handleMessage(const AddGfxEntityMessage& received);
        void handleMessage(const MoveGfxEntityMessage& received);
        void handleMessage(const RotateGfxEntityMessage& received);
        void handleMessage(const RemoveGfxEntityMessage& received);
        void handleMessage(const PlayerConnectedToEntityMessage& received);
        void handleMessage(const PlayerFacingBlockMessage& received);
        void handleMessage(const ChunkLoadedMessage& received);
        void handleMessage(const ChunkDeletedMessage& received);
        void handleMessage(const VoxelSetMessage& received);
    private:
        void handleClientPackage(uint32_t clientId, const std::unique_ptr<BasePackage>& package);

        fea::MessageBus& mBus;
        NetworkParameters mParameters;

        std::unordered_set<size_t> graphicsEntities; //temporary solution on how to resend things

        uint32_t mNextClientId;
        std::unordered_map<uint32_t, uint32_t> mClientToPlayerIds;
        std::unordered_map<uint32_t, uint32_t> mPlayerToClientIds;

        fea::MessageBus* mLocalClientBus;
        uint32_t mLocalPlayerId;

        std::unique_ptr<ENet> mENet;
        std::unique_ptr<ENetServer> mENetServer;
};
