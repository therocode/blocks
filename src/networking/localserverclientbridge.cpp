#include "localserverclientbridge.h"
#include <iostream>

LocalServerClientBridge::LocalServerClientBridge() : mOther(nullptr)
{
}

void LocalServerClientBridge::flush()
{
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
