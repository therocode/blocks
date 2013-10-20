#pragma once
#include "../world/world.h"
#include "../networking/serverclientbridge.h"
#include "../networking/clientconnection.h"
#include "../networking/clientconnectionlistener.h"
#include "../world/worldmessages.h"
#include "../rendering/renderingmessages.h"
#include "../script/scripthandler.h"
#include "../utilities/timer.h"
#include "../utilities/logger.h"

class Server : public fea::MessageReceiver<ChunkCreatedMessage>,
               public fea::MessageReceiver<AddGfxEntityMessage>,
               public fea::MessageReceiver<MoveGfxEntityMessage>,
               public fea::MessageReceiver<RemoveGfxEntityMessage>
{
    public:
        Server();
        ~Server();
        void setup();
        void doLogic();
        void destroy();
        void addClientBridge(std::unique_ptr<ServerClientBridge> clientBridge);
        void handleMessage(const ChunkCreatedMessage& received);
        void handleMessage(const AddGfxEntityMessage& received);
        void handleMessage(const MoveGfxEntityMessage& received);
        void handleMessage(const RemoveGfxEntityMessage& received);
    private:
        void acceptClientConnection(const std::shared_ptr<ClientConnection> client);
        void pollNewClients();
        void fetchClientData(std::weak_ptr<ClientConnection> client);
        fea::MessageBus mBus;
        Logger mLogger;
        World mWorld;
        ScriptHandler mScriptHandler;
        Timer mFrameTimer;
        std::string mLogName;

        std::map<ClientId, std::shared_ptr<ClientConnection> > mClients;
        std::unique_ptr<ClientConnectionListener> mListener;
};
