#pragma once
#include "world.h"
#include "serverclientbridge.h"

class Server
{
    public:
        Server();
        void setup();
        void doLogic();
        void destroy();
        void addClientBridge(std::unique_ptr<ServerClientBridge> clientBridge);
    private:
        fea::MessageBus mBus;
        World world;

        std::unique_ptr<ServerClientBridge> mBridge;      //this could be a list of many clients in the future
};
