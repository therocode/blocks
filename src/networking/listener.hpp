#pragma once
#include <queue>
#include <memory>
#include <mutex>
#include "clientconnection.hpp"

class Listener
{
    public:
        std::unique_ptr<ClientConnection> fetchIncomingConnection();
    protected:
        std::mutex mIncomingConnectionsMutex;
        std::queue<std::unique_ptr<ClientConnection>> mIncomingConnections;
};
