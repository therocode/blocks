#pragma once
#include <cstddef>
#include <memory>
#include "serverclientbridge.hpp"

class BasePackage;
using ClientId = size_t;

class ClientConnection
{
    public:
        ClientConnection(ClientId id);
        ClientId getId();
        void flush();
        void enqueuePackage(std::weak_ptr<BasePackage> package);
        bool pollPackage(std::shared_ptr<BasePackage>& package);
        void setBridge(std::unique_ptr<ServerClientBridge> bridge);
        bool isDisconnected();
    private:
        ClientId mId;

        std::unique_ptr<ServerClientBridge> mBridge;
};
