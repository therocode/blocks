#pragma once
#include <memory>
#include <enet/enet.h>
#include <fea/util.hpp>

class BasePackage;

class ENetServer
{
    public:
        ENetServer(fea::MessageBus& bus);
        ~ENetServer();
        void update();
        void sendToAll(std::unique_ptr<BasePackage> package);
        void sendToOne(uint32_t id, std::unique_ptr<BasePackage> package);
    private:
        std::unique_ptr<BasePackage> deserialize(ENetPacket* packet);
        fea::MessageBus& mBus;
        ENetAddress mAddress;
        ENetHost* mHost;
        bool mInitialized;
        uint32_t mNextId;

        std::unordered_map<uint32_t, ENetPeer*> mConnectedPeers;
};
