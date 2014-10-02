#pragma once
#include <enet/enet.h>
#include "enet.hpp"
#include <string>

class ENetClient
{
    public:
        ENetClient(const ENet& enet);
        ~ENetClient();
        void update(uint32_t wait);
        bool isConnected() const;
        void connect(const std::string& address, uint32_t port, uint32_t timeout);
        void disconnect(uint32_t timeout);
    private:
        ENetHost* mHost;
        ENetPeer* mServer;
        ENetAddress mAddress;
        bool mConnected;
};
