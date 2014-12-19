#include "clientresourcesystem.hpp"
#include "resourcepathtoname.hpp"
#include "resourcemessages.hpp"
#include "../utilities/folderexploder.hpp"
#include "../application/applicationmessages.hpp"
#include "../lognames.hpp"
#include "iqmfromfileloader.hpp"
#include "shadersource.hpp"
#include "shadersourcefromfileloader.hpp"
#include "shaderdefinition.hpp"
#include "shaderdefinitionfromfileloader.hpp"
#include "texturefromfileloader.hpp"

ClientResourceSystem::ClientResourceSystem(fea::MessageBus& bus, const std::string assetsPath) :
    mBus(bus),
    mAssetsPath(assetsPath)
{
    mBus.send(LogMessage{"Loading extensions...", resourceName, LogLevel::INFO});

    FolderExploder exploder;

    auto extensions = exploder.getSubFolders(assetsPath);

    for(const auto extensionName : extensions)
    {
        std::string extensionPath = assetsPath + "/" + extensionName;
        mBus.send(LogMessage{"Found extension '" + extensionPath + "'. Scanning for resources...", resourceName, LogLevel::INFO});
        std::vector<std::string> mResources;

        exploder.explodeFolder(extensionPath, {
                "iqm",
                "vert",
                "frag",
                "shad",
                "png"
                }, mResources);

        mBus.send(LogMessage{"Found " + std::to_string(mResources.size()) + " resources to load.", resourceName, LogLevel::INFO});

        for(auto& resourcePath : mResources)
        {
            std::string fileType = resourcePath.substr(resourcePath.find_last_of('.') + 1, std::string::npos);

            mResourceList[fileType].push_back({resourcePath, resourcePathToName(assetsPath, resourcePath)});
        }

        loadModels(mResourceList["iqm"]);
        loadVertexShaders(mResourceList["vert"]);
        loadFragmentShaders(mResourceList["frag"]);
        loadShaderDefinitions(mResourceList["shad"]);
        loadTextures(mResourceList["png"]);
    }
}

void ClientResourceSystem::loadModels(const std::vector<ResourceEntry>& models)
{
    mBus.send(LogMessage{"Loading models. " + std::to_string(models.size()) + " models to load.", resourceName, LogLevel::INFO});
    for(const auto& modelFile : models)
    {
        mBus.send(LogMessage{"Loading " + modelFile.name + ".", resourceName, LogLevel::VERB});
        std::shared_ptr<RawModel> model = mCache.access<IQMFromFileLoader>(modelFile.path);
        if(model)
        {
            uint32_t id = mModelIDs.getId(modelFile.name);
            mBus.send(ResourceDeliverMessage<RawModel>{id, model});
        }
    }
}

void ClientResourceSystem::loadVertexShaders(const std::vector<ResourceEntry>& vertexShaders)
{
    mBus.send(LogMessage{"Loading vertex shaders. " + std::to_string(vertexShaders.size()) + " vertex shaders to load.", resourceName, LogLevel::INFO});
    for(const auto& vertexShaderFile : vertexShaders)
    {
        mBus.send(LogMessage{"Loading " + vertexShaderFile.name + ".", resourceName, LogLevel::VERB});
        std::shared_ptr<ShaderSource> vertexShader = mCache.access<ShaderSourceFromFileLoader>(vertexShaderFile.path);
        if(vertexShader)
        {
            uint32_t id = mVertexShaderIDs.getId(vertexShaderFile.name);
            mBus.send(ResourceDeliverMessage<ShaderSource>{id, vertexShader});
        }
    }
}

void ClientResourceSystem::loadFragmentShaders(const std::vector<ResourceEntry>& fragmentShaders)
{
    mBus.send(LogMessage{"Loading fragment shaders. " + std::to_string(fragmentShaders.size()) + " fragment shaders to load.", resourceName, LogLevel::INFO});
    for(const auto& fragmentShaderFile : fragmentShaders)
    {
        mBus.send(LogMessage{"Loading " + fragmentShaderFile.name + ".", resourceName, LogLevel::VERB});
        std::shared_ptr<ShaderSource> fragmentShader = mCache.access<ShaderSourceFromFileLoader>(fragmentShaderFile.path);
        if(fragmentShader)
        {
            uint32_t id = mFragmentShaderIDs.getId(fragmentShaderFile.name);
            mBus.send(ResourceDeliverMessage<ShaderSource>{id, fragmentShader});
        }
    }
}

void ClientResourceSystem::loadShaderDefinitions(const std::vector<ResourceEntry>& shaderDefinitions)
{
    mBus.send(LogMessage{"Loading shader definitions. " + std::to_string(shaderDefinitions.size()) + " shader definitions to load.", resourceName, LogLevel::INFO});
    for(const auto& shaderDefinitionFile : shaderDefinitions)
    {
        mBus.send(LogMessage{"Loading " + shaderDefinitionFile.name + ".", resourceName, LogLevel::VERB});
        std::shared_ptr<ShaderDefinition> shaderDefinition = mCache.access<ShaderDefinitionFromFileLoader>(shaderDefinitionFile.path);
        if(shaderDefinition)
        {
            uint32_t id = mShaderDefinitionIDs.getId(shaderDefinitionFile.name);
            mBus.send(ResourceDeliverMessage<ShaderDefinition>{id, shaderDefinition});
        }
    }
}

void ClientResourceSystem::loadTextures(const std::vector<ResourceEntry>& textures)
{
    mBus.send(LogMessage{"Loading textures. " + std::to_string(textures.size()) + " textures to load.", resourceName, LogLevel::INFO});
    for(const auto& textureFile : textures)
    {
        mBus.send(LogMessage{"Loading " + textureFile.name + ".", resourceName, LogLevel::VERB});
        std::shared_ptr<Texture> texture = mCache.access<TextureFromFileLoader>(textureFile.path);

        if(texture)
        {
            uint32_t id = mTextureIDs.getId(textureFile.name);
            mBus.send(ResourceDeliverMessage<Texture>{id, texture});
        }
    }
}
