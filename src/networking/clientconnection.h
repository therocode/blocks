#pragma once
#include <cstddef>
#include <memory>
#include "package.h"
#include "serverclientbridge.h"

using ClientId = size_t;

class ClientConnection
{
    public:
        ClientConnection();
        ClientConnection(ClientId id);
        ClientId getId();
        void flush();
        void enqueuePackage(std::weak_ptr<BasePackage> package);
        bool pollPackage(std::shared_ptr<BasePackage>& package);
    private:
        ClientId mId;

        std::unique_ptr<ServerClientBridge> mBridge;
};
