#pragma once
#include "world.h"
#include "serverclientbridge.h"

class Server : public fea::MessageReceiver<ChunkCreatedMessage>
{
    public:
        Server();
        void setup();
        void doLogic();
        void destroy();
        void addClientBridge(std::unique_ptr<ServerClientBridge> clientBridge);
        void handleMessage(const ChunkCreatedMessage& received);
    private:
        fea::MessageBus mBus;
        World world;

        std::unique_ptr<ServerClientBridge> mBridge;      //this could be a list of many clients in the future
};
