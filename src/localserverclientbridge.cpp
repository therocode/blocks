#include "localserverclientbridge.h"
#include <iostream>

void LocalServerClientBridge::flush()
{
    for(uint32_t i = 0; i < mOutgoing.size(); i++)
    {
        mOther->receivePackage(std::move(mOutgoing[i]));
    }

    mOutgoing.clear();
}

void LocalServerClientBridge::connect(LocalServerClientBridge* other)
{
    mOther = other;
}

void LocalServerClientBridge::receivePackage(std::unique_ptr<Package>&& incoming)
{
    mIncoming.push_back(std::move(incoming));
}
