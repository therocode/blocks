#pragma once
#include <cstddef>
#include <memory>
#include "package.h"
#include "serverclientbridge.h"

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
    private:
        ClientId mId;

        std::unique_ptr<ServerClientBridge> mBridge;
};