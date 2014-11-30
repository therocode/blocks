#pragma once
#include <fea/util.hpp>
#include "resourcecache.hpp"

class ClientResourceSystem
{
    public:
        ClientResourceSystem(fea::MessageBus& bus);
    private:
        fea::MessageBus& mBus;
        ResourceCache mCache;
};
