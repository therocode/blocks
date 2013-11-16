#include "serverclientbridge.h"

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
    mOutGoingMutex.lock();
    mOutgoing.push_back(package.lock());
    mOutGoingMutex.unlock();
}
