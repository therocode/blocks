#pragma once
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>
#include <enet/enet.h>
#include "enet.hpp"

class ENetServer
{
    public:
        ENetServer(const ENet& enet, uint32_t port);
        ~ENetServer();
        void update(uint32_t wait);
        void sendToAll(const std::vector<uint8_t>& data, bool reliable, uint8_t channel);
        void sendToOne(uint32_t id, const std::vector<uint8_t>& data, bool reliable, uint8_t channel);
        bool isListening() const;
        uint32_t getClientCount() const;
        void setConnectionCallback(std::function<void(uint32_t)> callback);
        void setDataReceivedCallback(std::function<void(uint32_t, const std::vector<uint8_t>&)> callback);
        void setDisconnectionCallback(std::function<void(uint32_t)> callback);
    private:
        std::vector<uint8_t> deserialize(ENetPacket* packet);
        ENetAddress mAddress;
        ENetHost* mHost;
        uint32_t mNextId;
        uint32_t mClientCount;

        std::unordered_map<uint32_t, ENetPeer*> mConnectedPeers;

        std::function<void(uint32_t)> mConnectionCallback;
        std::function<void(uint32_t, const std::vector<uint8_t>&)> mDataReceivedCallback;
        std::function<void(uint32_t)> mDisconnectionCallback;
};
