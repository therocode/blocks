#pragma once
#include "../world/universe.h"
#include "../networking/serverclientbridge.h"
#include "../networking/clientconnection.h"
#include "../networking/clientconnectionlistener.h"
#include "../world/worldmessages.h"
#include "../rendering/renderingmessages.h"
#include "../script/scripthandler.h"
#include "../utilities/fpscontroller.h"
#include "../utilities/logger.h"

class Server : public fea::MessageReceiver<FatalMessage>,
               public fea::MessageReceiver<AddGfxEntityMessage>,
               public fea::MessageReceiver<MoveGfxEntityMessage>,
               public fea::MessageReceiver<RotateGfxEntityMessage>,
               public fea::MessageReceiver<RemoveGfxEntityMessage>,
               public fea::MessageReceiver<PlayerConnectedToEntityMessage>,
               public fea::MessageReceiver<PlayerFacingBlockMessage>,
               public fea::MessageReceiver<VoxelSetMessage>
{
    public:
        Server();
        ~Server();
        void setup();
        void doLogic();
        void destroy();
        void handleMessage(const FatalMessage& received);
        void handleMessage(const AddGfxEntityMessage& received);
        void handleMessage(const MoveGfxEntityMessage& received);
        void handleMessage(const RotateGfxEntityMessage& received);
        void handleMessage(const RemoveGfxEntityMessage& received);
        void handleMessage(const PlayerConnectedToEntityMessage& received);
        void handleMessage(const PlayerFacingBlockMessage& received);
        void handleMessage(const VoxelSetMessage& received);
        void setClientListener(std::unique_ptr<ClientConnectionListener> clientListener);
		fea::MessageBus& getBus();
    private:
        void acceptClientConnection(const std::shared_ptr<ClientConnection> client);
        void pollNewClients();
        void fetchClientData(std::weak_ptr<ClientConnection> client);
        void checkForDisconnectedClients();
        fea::MessageBus mBus;
        Logger mLogger;
        Universe mUniverse;
        ScriptHandler mScriptHandler;
        std::string mLogName;
        FPSController mFPSController;

        std::map<ClientId, std::shared_ptr<ClientConnection> > mClients;
        std::unique_ptr<ClientConnectionListener> mListener;

        std::set<size_t> graphicsEntities; //temporary solution on how to resend things
};
