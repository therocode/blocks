#pragma once
#include "../blockstd.h"
#include "serverclientbridge.h"
#include "enet/enet.h"
#include <atomic>

class RemoteClientBridge : public ServerClientBridge
{
    public:
		RemoteClientBridge(ENetPeer* peer);
        virtual void flush() override;
        void receivePackage(std::weak_ptr<BasePackage> incoming);
        void acceptEnetPacket(ENetPacket* packet);
        void sendAwaiting();
    private:
        void deserialiseAndReceive(const std::vector<uint8_t>& data, BasePackage* package);
		ENetPeer* mPeer;
        
        std::atomic<bool> mGotPackagesToSend;
};
