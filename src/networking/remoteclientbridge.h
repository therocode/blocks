#pragma once
#include "../blockstd.h"
#include "serverclientbridge.h"
#include "enet/enet.h"

class RemoteClientBridge : public ServerClientBridge
{
    public:
		RemoteClientBridge(ENetPeer* peer);
        virtual void flush() override;
        void receivePackage(std::weak_ptr<BasePackage> incoming);
        void acceptEnetPacket(ENetPacket* packet);
    private:
		ENetPeer* mPeer;
};