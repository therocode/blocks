#pragma once
#include "world.h"
#include "serverclientbridge.h"
#include "messages.h"
#include "script/scripthandler.h"
#include "timer.h"
#include "logger.h"

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
        void fetchClientData();
        fea::MessageBus mBus;
        Logger mLogger;
        World mWorld;
        ScriptHandler mScriptHandler;
        Timer mFrameTimer;
        std::string mLogName;

        std::unique_ptr<ServerClientBridge> mBridge;      //this could be a list of many clients in the future
};
