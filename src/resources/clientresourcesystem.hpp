#pragma once
#include <fea/util.hpp>
#include "resourcecache.hpp"

class ClientResourceSystem
{
    public:
        ClientResourceSystem(fea::MessageBus& bus);
    private:
        void loadModels(const std::vector<std::string>& models);
        void loadVertexShaders(const std::vector<std::string>& vertexShaders);
        void loadFragmentShaders(const std::vector<std::string>& fragmentShaders);
        void loadShaderDefinitions(const std::vector<std::string>& shaderDefinitions);
        fea::MessageBus& mBus;
        ResourceCache mCache;

        std::unordered_map<std::string, std::vector<std::string>> mResourceList;
};
