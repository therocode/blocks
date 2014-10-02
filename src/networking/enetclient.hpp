#pragma once
#include <enet/enet.h>
#include "enet.hpp"
#include <string>
#include <vector>
#include <functional>

class ENetClient
{
    public:
        ENetClient(const ENet& enet);
        ~ENetClient();
        void update(uint32_t wait);
        bool isConnected() const;
        void connect(const std::string& address, uint32_t port, uint32_t timeout);
        void disconnect(uint32_t timeout);
        void send(const std::vector<uint8_t>& data, bool reliable, uint8_t channel);
        void setConnectedCallback(std::function<void()> callback);
        void setDataReceivedCallback(std::function<void(const std::vector<uint8_t>&)> callback);
        void setDisconnectedCallback(std::function<void()> callback);
    private:
        std::vector<uint8_t> unpack(ENetPacket* packet);
        ENetHost* mHost;
        ENetPeer* mServer;
        ENetAddress mAddress;
        bool mConnected;
        std::function<void()> mConnectedCallback;
        std::function<void(const std::vector<uint8_t>&)> mDataReceivedCallback;
        std::function<void()> mDisconnectedCallback;
};
