#include "enet.hpp"
#include <enet/enet.h>

ENet::ENet()
{
    if(enet_initialize() != 0)
    {
        mInitialized = true;
    }
}

ENet::~ENet()
{
    if(mInitialized)
        enet_deinitialize();
}

bool ENet::isInitialized() const
{
    return mInitialized;
}
