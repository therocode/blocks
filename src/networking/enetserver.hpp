#pragma once
#include <memory>
#include <unordered_map>
#include <enet/enet.h>
#include "enet.hpp"

class BasePackage;

class ENetServer
{
    public:
        ENetServer(const ENet& enet, uint32_t port);
        ~ENetServer();
        void update(uint32_t wait);
        void sendToAll(std::unique_ptr<BasePackage> package);
        void sendToOne(uint32_t id, std::unique_ptr<BasePackage> package);
        bool isListening() const;
        uint32_t getClientCount() const;
    private:
        std::unique_ptr<BasePackage> deserialize(ENetPacket* packet);
        ENetAddress mAddress;
        ENetHost* mHost;
        uint32_t mNextId;
        uint32_t mClientCount;

        std::unordered_map<uint32_t, ENetPeer*> mConnectedPeers;
};
