#pragma once
#include <enet/enet.h>
#include <fea/util.hpp>
#include "enet.hpp"

class ENetClient
{
    public:
        ENetClient(const ENet& enet, fea::MessageBus& bus);
        ~ENetClient();
    private:
        fea::MessageBus& mBus;
        ENetHost* mHost;
        ENetPeer* mServer;
};
