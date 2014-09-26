#pragma once
#include "../world/worldsystem.hpp"
#include "../networking/serverclientbridge.hpp"
#include "../world/worldmessages.hpp"
#include "../rendering/renderingmessages.hpp"
#include "../script/scriptsystem.hpp"
#include "../utilities/fpscontroller.hpp"
#include "../utilities/logger.hpp"
#include "../entity/entitysystem.hpp"

class ClientConnection;
class ClientConnectionListener;
using ClientId = size_t;

class Server : public fea::MessageReceiver<AddGfxEntityMessage,
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
        Server(fea::MessageBus& bus);
        ~Server();
        void setup();
        void doLogic();
        void handleMessage(const AddGfxEntityMessage& received);
        void handleMessage(const MoveGfxEntityMessage& received);
        void handleMessage(const RotateGfxEntityMessage& received);
        void handleMessage(const RemoveGfxEntityMessage& received);
        void handleMessage(const PlayerConnectedToEntityMessage& received);
        void handleMessage(const PlayerFacingBlockMessage& received);
        void handleMessage(const ChunkLoadedMessage& received);
        void handleMessage(const ChunkDeletedMessage& received);
        void handleMessage(const VoxelSetMessage& received);
        void setClientListener(std::unique_ptr<ClientConnectionListener> clientListener);
    private:
        void acceptClientConnection(const std::shared_ptr<ClientConnection> client);
        void pollNewClients();
        void fetchClientData(std::weak_ptr<ClientConnection> client);
        void checkForDisconnectedClients();
        fea::MessageBus& mBus;
        Logger mLogger;
        std::map<ClientId, std::shared_ptr<ClientConnection> > mClients;
        std::unique_ptr<ClientConnectionListener> mListener;
        WorldSystem mWorlds;
        ScriptSystem mScriptSystem;
        EntitySystem mEntitySystem;
        FPSController mFPSController;

        std::set<size_t> graphicsEntities; //temporary solution on how to resend things
};
