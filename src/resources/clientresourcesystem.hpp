#pragma once
#include <fea/util.hpp>
#include <string>
#include "resourcecache.hpp"
#include "resourceentry.hpp"
#include "../utilities/idprovider.hpp"
#include "../rendering/texturearray.hpp"

struct TextureDefinition
{
    uint32_t textureArrayId;
    uint32_t index;
};

class ClientResourceSystem
{
    public:
        ClientResourceSystem(fea::MessageBus& bus, const std::string assetsPath);
    private:
        void loadModels(const std::vector<ResourceEntry>& models);
        void loadVertexShaders(const std::vector<ResourceEntry>& vertexShaders);
        void loadFragmentShaders(const std::vector<ResourceEntry>& fragmentShaders);
        void loadShaderDefinitions(const std::vector<ResourceEntry>& shaderDefinitions);
        void loadImages(const std::vector<ResourceEntry>& images);
        fea::MessageBus& mBus;
        std::string mAssetsPath;
        ResourceCache mCache;

        std::unordered_map<std::string, std::vector<ResourceEntry>> mResourceList;

        IdProvider<std::string> mModelIDs;
        IdProvider<std::string> mVertexShaderIDs;
        IdProvider<std::string> mFragmentShaderIDs;
        IdProvider<std::string> mShaderDefinitionIDs;
        IdProvider<std::string> mTextureIDs;
        uint32_t mNextTextureId;
        std::unordered_map<uint32_t, std::shared_ptr<TextureArray>> mTextureArrays;
        std::unordered_map<uint32_t, TextureDefinition> mTextureDefinitions;
};
