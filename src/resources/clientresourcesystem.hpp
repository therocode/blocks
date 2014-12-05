#pragma once
#include <fea/util.hpp>
#include "resourcecache.hpp"

class ClientResourceSystem
{
    public:
        ClientResourceSystem(fea::MessageBus& bus);
    private:
        void loadModels(const std::vector<std::string>& models);
        fea::MessageBus& mBus;
        ResourceCache mCache;

        std::unordered_map<std::string, std::vector<std::string>> mResourceList;
};
