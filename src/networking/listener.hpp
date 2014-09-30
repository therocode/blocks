#pragma once
#include <queue>
#include <memory>
#include <mutex>

class ServerClientBridge;

class Listener
{
    public:
        std::unique_ptr<ServerClientBridge> fetchIncomingConnection();
    protected:
        std::mutex mIncomingConnectionsMutex;
        std::queue<std::unique_ptr<ServerClientBridge>> mIncomingConnections;
};
