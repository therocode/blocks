#include "serverclientbridge.h"

bool ServerClientBridge::pollPackage(std::unique_ptr<BasePackage>& package)
{
    if(mIncoming.size())
    {
        package = std::move(mIncoming.front());
        mIncoming.pop_front();
        return true;
    }
    else
    {
        return false;
    }
}

void ServerClientBridge::enqueuePackage(std::unique_ptr<BasePackage>&& package)
{
    mOutgoing.push_back(std::move(package));
}
