#include "clientresourcesystem.hpp"
#include "resourcemessages.hpp"
#include "../utilities/folderexploder.hpp"
#include "../application/applicationmessages.hpp"
#include "../lognames.hpp"
#include "iqmfromfileloader.hpp"
#include "shadersource.hpp"
#include "shadersourcefromfileloader.hpp"
#include "shaderdefinition.hpp"
#include "shaderdefinitionfromfileloader.hpp"

ClientResourceSystem::ClientResourceSystem(fea::MessageBus& bus) :
    mBus(bus)
{
    mBus.send(LogMessage{"Scanning 'assets' for resources...", resourceName, LogLevel::INFO});

    FolderExploder exploder;

    std::vector<std::string> mResources;

    exploder.explodeFolder("assets", 
".*\\.iqm|\
.*\\.vert|\
.*\\.frag|\
.*\\.shad|\
.*\\.png", mResources);

    mBus.send(LogMessage{"Found " + std::to_string(mResources.size()) + " resources to load.", resourceName, LogLevel::INFO});

    for(auto& resourcePath : mResources)
    {
        std::string fileType = resourcePath.substr(resourcePath.find_last_of('.') + 1, std::string::npos);

        mResourceList[fileType].push_back(resourcePath);
    }

    loadModels(mResourceList["iqm"]);
    loadVertexShaders(mResourceList["vert"]);
    loadFragmentShaders(mResourceList["frag"]);
    loadShaderDefinitions(mResourceList["shad"]);
    loadTextures(mResourceList["png"]);
}

void ClientResourceSystem::loadModels(const std::vector<std::string>& models)
{
    mBus.send(LogMessage{"Loading models. " + std::to_string(models.size()) + " models to load.", resourceName, LogLevel::INFO});
    for(const auto& path : models)
    {
        mBus.send(LogMessage{"Loading " + path + ".", resourceName, LogLevel::VERB});
        std::shared_ptr<RawModel> model = mCache.access<IQMFromFileLoader>(path);
        if(model)
        {
            mBus.send(ModelDeliverMessage{model});
        }
    }
}

void ClientResourceSystem::loadVertexShaders(const std::vector<std::string>& vertexShaders)
{
    mBus.send(LogMessage{"Loading vertex shaders. " + std::to_string(vertexShaders.size()) + " vertex shaders to load.", resourceName, LogLevel::INFO});
    for(const auto& path : vertexShaders)
    {
        mBus.send(LogMessage{"Loading " + path + ".", resourceName, LogLevel::VERB});
        std::shared_ptr<ShaderSource> vertexShader = mCache.access<ShaderSourceFromFileLoader>(path);
        if(vertexShader)
        {
            mBus.send(ShaderSourceDeliverMessage{vertexShader});
        }
    }
}

void ClientResourceSystem::loadFragmentShaders(const std::vector<std::string>& fragmentShaders)
{
    mBus.send(LogMessage{"Loading fragment shaders. " + std::to_string(fragmentShaders.size()) + " fragment shaders to load.", resourceName, LogLevel::INFO});
    for(const auto& path : fragmentShaders)
    {
        mBus.send(LogMessage{"Loading " + path + ".", resourceName, LogLevel::VERB});
        std::shared_ptr<ShaderSource> fragmentShader = mCache.access<ShaderSourceFromFileLoader>(path);
        if(fragmentShader)
        {
            mBus.send(ShaderSourceDeliverMessage{fragmentShader});
        }
    }
}

void ClientResourceSystem::loadShaderDefinitions(const std::vector<std::string>& shaderDefinitions)
{
    mBus.send(LogMessage{"Loading shader definitions. " + std::to_string(shaderDefinitions.size()) + " shader definitions to load.", resourceName, LogLevel::INFO});
    for(const auto& path : shaderDefinitions)
    {
        mBus.send(LogMessage{"Loading " + path + ".", resourceName, LogLevel::VERB});
        std::shared_ptr<ShaderDefinition> shaderDefinition = mCache.access<ShaderDefinitionFromFileLoader>(path);
        if(shaderDefinition)
        {
            mBus.send(ShaderDefinitionDeliverMessage{shaderDefinition});
        }
    }
}

void ClientResourceSystem::loadTextures(const std::vector<std::string>& textures)
{
    mBus.send(LogMessage{"Loading textures. " + std::to_string(textures.size()) + " textures to load.", resourceName, LogLevel::INFO});
    for(const auto& path : textures)
    {
        mBus.send(LogMessage{"Loading " + path + ".", resourceName, LogLevel::VERB});
    //    std::shared_ptr<Texture> textures = mCache.access<TextureFromFileLoader>(path);
    //    if(texture)
    //    {
    //        mTextureIDs.getId(path);
    //        mBus.send(TextureDeliverMessage{texture});
    //    }
    }
}
