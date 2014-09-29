#pragma once
#include <queue>
#include <memory>
#include <mutex>

class ClientConnection;

class Listener
{
    public:
        std::shared_ptr<ClientConnection> fetchIncomingConnection();
    protected:
        std::mutex mIncomingConnectionsMutex;
        std::queue<std::shared_ptr<ClientConnection> > mIncomingConnections;
};
