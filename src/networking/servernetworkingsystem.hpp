#pragma once
#include <fea/util.hpp>
#include <memory>
#include <vector>
#include <unordered_map>
#include "networkparameters.hpp"
#include "listener.hpp"
#include "../rendering/renderingmessages.hpp"
#include "../entity/entitymessages.hpp"
#include "../world/worldmessages.hpp"
#include "../application/applicationmessages.hpp"
#include "networkingmessages.hpp"

class ClientConnection;
using ClientId = size_t;

class ServerNetworkingSystem : public fea::MessageReceiver<LocalConnectionAttemptMessage,
                                           FrameMessage,
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
        void acceptClientConnection(const std::shared_ptr<ClientConnection> client);
        void pollNewClients();
        void fetchClientData(std::weak_ptr<ClientConnection> client);
        void checkForDisconnectedClients();

        fea::MessageBus& mBus;
        NetworkParameters mParameters;

        std::unordered_set<size_t> graphicsEntities; //temporary solution on how to resend things

        std::unordered_map<ClientId, std::shared_ptr<ClientConnection> > mClients;
        std::vector<std::unique_ptr<Listener>> mListeners;
};
