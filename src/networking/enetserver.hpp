#pragma once
#include <memory>
#include <functional>
#include <unordered_map>
#include <enet/enet.h>
#include "enet.hpp"
#include "packages.hpp"

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
        void setConnectionCallback(std::function<void(uint32_t)> callback);
        void setPackageReceivedCallback(std::function<void(uint32_t, const std::unique_ptr<BasePackage>&)> callback);
        void setDisconnectionCallback(std::function<void(uint32_t)> callback);
        void setUnknownPackageCallback(std::function<void(PackageType)> callback);
    private:
        std::unique_ptr<BasePackage> deserialize(ENetPacket* packet);
        ENetAddress mAddress;
        ENetHost* mHost;
        uint32_t mNextId;
        uint32_t mClientCount;

        std::unordered_map<uint32_t, ENetPeer*> mConnectedPeers;

        std::function<void(uint32_t)> mConnectionCallback;
        std::function<void(uint32_t, const std::unique_ptr<BasePackage>&)> mPackageReceivedCallback;
        std::function<void(uint32_t)> mDisconnectionCallback;
        std::function<void(PackageType)> mUnknownPackageCallback;
};
