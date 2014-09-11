#include "localserverclientbridge.hpp"

LocalServerClientBridge::LocalServerClientBridge() : mOther(nullptr)
{
}

void LocalServerClientBridge::flush()
{
    std::lock_guard<std::mutex> lock(mOutGoingMutex);
    if(mOther)
    {
        for(uint32_t i = 0; i < mOutgoing.size(); i++)
        {
            mOther->receivePackage(std::move(mOutgoing[i]));
        }
    }

    mOutgoing.clear();
}

void LocalServerClientBridge::connect(LocalServerClientBridge* other)
{
    mOther = other;
}

void LocalServerClientBridge::receivePackage(std::weak_ptr<BasePackage> incoming)
{
    mIncoming.push_back(incoming.lock());
}
