#pragma once
#include <fea/util.hpp>
#include <string>
#include "resourcecache.hpp"
#include "resourceentry.hpp"
#include "../utilities/idmapper.hpp"
#include "../rendering/texturearray.hpp"
#include "../world/worldparameters.hpp"

struct TextureDefinition
{
    uint32_t textureArrayId;
    uint32_t index;
};

class ResourceSystem
{
    public:
        ResourceSystem(fea::MessageBus& bus, const std::string assetsPath, const std::vector<std::string> fileTypes);
    private:
        void loadModels(const std::vector<ResourceEntry>& models);
        void loadVertexShaders(const std::vector<ResourceEntry>& vertexShaders);
        void loadFragmentShaders(const std::vector<ResourceEntry>& fragmentShaders);
        void loadShaderDefinitions(const std::vector<ResourceEntry>& shaderDefinitions);
        void loadExtensionMetadata(const std::vector<ResourceEntry>& extensionMetadata);
        void loadWorlds(const std::vector<ResourceEntry>& worlds);
        void loadImages(const std::vector<ResourceEntry>& images);

        fea::MessageBus& mBus;
        std::string mAssetsPath;
        ResourceCache mCache;

        std::unordered_map<std::string, std::vector<ResourceEntry>> mResourceList;

        IdMapper<std::string> mModelIDs;
        IdMapper<std::string> mVertexShaderIDs;
        IdMapper<std::string> mFragmentShaderIDs;
        IdMapper<std::string> mShaderDefinitionIDs;
        IdMapper<std::string> mTextureIDs;
        IdMapper<std::string> mExtensionMetadataIDs;

        uint32_t mNextTextureId;
        std::unordered_map<uint32_t, std::shared_ptr<TextureArray>> mTextureArrays;
        std::unordered_map<uint32_t, TextureDefinition> mTextureDefinitions;
};
