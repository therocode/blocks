#include "serverclientbridge.hpp"

bool ServerClientBridge::pollPackage(std::shared_ptr<BasePackage>& package)
{
    if(mIncoming.size())
    {
        package = mIncoming.front();
        mIncoming.pop_front();
        return true;
    }
    else
    {
        return false;
    }
}

void ServerClientBridge::enqueuePackage(std::weak_ptr<BasePackage> package)
{
    std::lock_guard<std::mutex> lock(mOutGoingMutex);
    mOutgoing.push_back(package.lock());
}

ServerClientBridge::~ServerClientBridge()
{
}

void ServerClientBridge::disconnect()
{
    disconnected = true;
}

bool ServerClientBridge::isDisconnected()
{
    return disconnected;
}
