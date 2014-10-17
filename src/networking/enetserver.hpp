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
        ENetServer(const ENet& enet, uint32_t port, int32_t channelAmount);
        ~ENetServer();
        void update(uint32_t wait);
        void sendToAll(const std::vector<uint8_t>& data, bool reliable, uint8_t channel);
        void sendToOne(uint32_t id, const std::vector<uint8_t>& data, bool reliable, uint8_t channel);
        void disconnectOne(uint32_t id, uint32_t wait);
        void disconnectAll(uint32_t wait);
        bool isListening() const;
        uint32_t getClientCount() const;
        void setConnectedCallback(std::function<void(uint32_t)> callback);
        void setDataReceivedCallback(std::function<void(uint32_t, const std::vector<uint8_t>&)> callback);
        void setDisconnectedCallback(std::function<void(uint32_t)> callback);
    private:
        void handleConnectionEvent(const ENetEvent& event);
        void handleReceiveEvent(const ENetEvent& event);
        void handleDisconnectionEvent(const ENetEvent& event);

        std::vector<uint8_t> unpack(ENetPacket* packet);
        ENetAddress mAddress;
        ENetHost* mHost;
        uint32_t mNextId;
        uint32_t mClientCount;

        std::unordered_map<uint32_t, ENetPeer*> mConnectedPeers;

        std::function<void(uint32_t)> mConnectedCallback;
        std::function<void(uint32_t, const std::vector<uint8_t>&)> mDataReceivedCallback;
        std::function<void(uint32_t)> mDisconnectedCallback;
};
