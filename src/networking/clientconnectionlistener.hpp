#pragma once
#include <queue>
#include <memory>
#include "clientconnection.hpp"
#include <mutex>

class ClientConnectionListener
{
    public:
        std::shared_ptr<ClientConnection> fetchIncomingConnection();
    protected:
        std::mutex mIncomingConnectionsMutex;
        std::queue<std::shared_ptr<ClientConnection> > mIncomingConnections;
};