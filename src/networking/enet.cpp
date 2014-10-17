#include "enet.hpp"
#include <enet/enet.h>
#include <cstdint>

ENet::ENet() :
    mInitialized(false)
{
    if(int32_t returned = enet_initialize() == 0)
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
