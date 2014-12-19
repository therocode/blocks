#pragma once
#include <fea/util.hpp>
#include <string>
#include "resourcecache.hpp"
#include "resourceentry.hpp"
#include "../utilities/idprovider.hpp"

class ClientResourceSystem
{
    public:
        ClientResourceSystem(fea::MessageBus& bus, const std::string assetsPath);
    private:
        void loadModels(const std::vector<ResourceEntry>& models);
        void loadVertexShaders(const std::vector<ResourceEntry>& vertexShaders);
        void loadFragmentShaders(const std::vector<ResourceEntry>& fragmentShaders);
        void loadShaderDefinitions(const std::vector<ResourceEntry>& shaderDefinitions);
        void loadTextures(const std::vector<ResourceEntry>& textures);
        fea::MessageBus& mBus;
        std::string mAssetsPath;
        ResourceCache mCache;

        std::unordered_map<std::string, std::vector<ResourceEntry>> mResourceList;

        IdProvider<std::string> mTextureIDs;
};
