#include "clientconnection.hpp"

//ClientConnection::ClientConnection() : mId(-1)
//{
//}

ClientConnection::ClientConnection(ClientId id) : mId(id)
{
}

ClientId ClientConnection::getId()
{
    return mId;
}

void ClientConnection::flush()
{
    mBridge->flush();
}

void ClientConnection::enqueuePackage(std::weak_ptr<BasePackage> package)
{
    mBridge->enqueuePackage(package);
}

bool ClientConnection::pollPackage(std::shared_ptr<BasePackage>& package)
{
    return mBridge->pollPackage(package);
}

void ClientConnection::setBridge(std::unique_ptr<ServerClientBridge> bridge)
{
    mBridge = std::move(bridge);
}

bool ClientConnection::isDisconnected()
{
    return mBridge->isDisconnected();
}
