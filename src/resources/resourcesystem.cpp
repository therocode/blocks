#include "resourcesystem.hpp"
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
#include "extensionmetadata.hpp"
#include "extensionmetadatafromfileloader.hpp"
#include "imagefromfileloader.hpp"
#include "../utilities/glmhash.hpp"
#include "worldfromfileloader.hpp"
#include "resourceexception.hpp"

ResourceSystem::ResourceSystem(fea::MessageBus& bus, const std::string assetsPath, const std::vector<std::string> fileTypes) :
    mBus(bus),
    mAssetsPath(assetsPath),
    mNextTextureId(0)
{
    mBus.send(LogMessage{"Loading extensions...", gResourceName, LogLevel::INFO});

    FolderExploder exploder;

    auto extensions = exploder.getSubFolders(assetsPath);

    for(const auto extensionName : extensions)
    {
        std::string extensionPath = assetsPath + "/" + extensionName;
        mBus.send(LogMessage{"Found extension '" + extensionPath + "'. Scanning for resources...", gResourceName, LogLevel::INFO});
        std::vector<std::string> mResources;

        exploder.explodeFolder(extensionPath, fileTypes, mResources);

        mBus.send(LogMessage{"Found " + std::to_string(mResources.size()) + " resources to load.", gResourceName, LogLevel::INFO});

        for(auto& resourcePath : mResources)
        {
            std::string fileType = resourcePath.substr(resourcePath.find_last_of('.') + 1, std::string::npos);

            mResourceList[fileType].push_back({resourcePath, resourcePathToName(assetsPath, resourcePath)});
        }

        if(std::find(fileTypes.begin(), fileTypes.end(), "meta") != fileTypes.end())
            loadExtensionMetadata(mResourceList["meta"]);
        if(std::find(fileTypes.begin(), fileTypes.end(), "iqm") != fileTypes.end())
            loadModels(mResourceList["iqm"]);
        if(std::find(fileTypes.begin(), fileTypes.end(), "vert") != fileTypes.end())
            loadVertexShaders(mResourceList["vert"]);
        if(std::find(fileTypes.begin(), fileTypes.end(), "frag") != fileTypes.end())
            loadFragmentShaders(mResourceList["frag"]);
        if(std::find(fileTypes.begin(), fileTypes.end(), "shad") != fileTypes.end())
            loadShaderDefinitions(mResourceList["shad"]);
        if(std::find(fileTypes.begin(), fileTypes.end(), "png") != fileTypes.end())
            loadImages(mResourceList["png"]);
		if(std::find(fileTypes.begin(), fileTypes.end(), "wld") != fileTypes.end())
            loadWorlds(mResourceList["wld"]);
    }
}

void ResourceSystem::loadModels(const std::vector<ResourceEntry>& models)
{
    mBus.send(LogMessage{"Loading models. " + std::to_string(models.size()) + " models to load.", gResourceName, LogLevel::INFO});
    for(const auto& modelFile : models)
    {
        try
        {
            mBus.send(LogMessage{"Loading " + modelFile.name + ".", gResourceName, LogLevel::VERB});
            std::shared_ptr<RawModel> model = mCache.access<IQMFromFileLoader>(modelFile.path);
            if(model)
            {
                uint32_t id = mModelIDs.getId(modelFile.name);
                mBus.send(ResourceDeliverMessage<RawModel>{id, model});
            }
        }
        catch(ResourceException& exception)
        {
            mBus.send(LogMessage{modelFile.name + ": " + exception.what(), gResourceName, LogLevel::ERR});
        }
    }
}

void ResourceSystem::loadVertexShaders(const std::vector<ResourceEntry>& vertexShaders)
{
    mBus.send(LogMessage{"Loading vertex shaders. " + std::to_string(vertexShaders.size()) + " vertex shaders to load.", gResourceName, LogLevel::INFO});
    for(const auto& vertexShaderFile : vertexShaders)
    {
        try
        {
            mBus.send(LogMessage{"Loading " + vertexShaderFile.name + ".", gResourceName, LogLevel::VERB});
            std::shared_ptr<ShaderSource> vertexShader = mCache.access<ShaderSourceFromFileLoader>(vertexShaderFile.path);
            if(vertexShader)
            {
                uint32_t id = mVertexShaderIDs.getId(vertexShaderFile.name);
                vertexShader->name = vertexShaderFile.name;
                mBus.send(ResourceDeliverMessage<ShaderSource>{id, vertexShader});
            }
        }
        catch(ResourceException& exception)
        {
            mBus.send(LogMessage{vertexShaderFile.name + ": " + exception.what(), gResourceName, LogLevel::ERR});
        }
    }
}

void ResourceSystem::loadFragmentShaders(const std::vector<ResourceEntry>& fragmentShaders)
{
    mBus.send(LogMessage{"Loading fragment shaders. " + std::to_string(fragmentShaders.size()) + " fragment shaders to load.", gResourceName, LogLevel::INFO});
    for(const auto& fragmentShaderFile : fragmentShaders)
    {
        try
        {
            mBus.send(LogMessage{"Loading " + fragmentShaderFile.name + ".", gResourceName, LogLevel::VERB});
            std::shared_ptr<ShaderSource> fragmentShader = mCache.access<ShaderSourceFromFileLoader>(fragmentShaderFile.path);
            if(fragmentShader)
            {
                uint32_t id = mFragmentShaderIDs.getId(fragmentShaderFile.name);
                fragmentShader->name = fragmentShaderFile.name;
                mBus.send(ResourceDeliverMessage<ShaderSource>{id, fragmentShader});
            }
        }
        catch(ResourceException& exception)
        {
            mBus.send(LogMessage{fragmentShaderFile.name + ": " + exception.what(), gResourceName, LogLevel::ERR});
        }
    }
}

void ResourceSystem::loadShaderDefinitions(const std::vector<ResourceEntry>& shaderDefinitions)
{
    mBus.send(LogMessage{"Loading shader definitions. " + std::to_string(shaderDefinitions.size()) + " shader definitions to load.", gResourceName, LogLevel::INFO});
    for(const auto& shaderDefinitionFile : shaderDefinitions)
    {
        try
        {
            mBus.send(LogMessage{"Loading " + shaderDefinitionFile.name + ".", gResourceName, LogLevel::VERB});
            std::shared_ptr<ShaderDefinition> shaderDefinition = mCache.access<ShaderDefinitionFromFileLoader>(shaderDefinitionFile.path);
            if(shaderDefinition)
            {
                uint32_t id = mShaderDefinitionIDs.getId(shaderDefinitionFile.name);
                mBus.send(ResourceDeliverMessage<ShaderDefinition>{id, shaderDefinition});
            }
        }
        catch(ResourceException& exception)
        {
            mBus.send(LogMessage{shaderDefinitionFile.name + ": " + exception.what(), gResourceName, LogLevel::ERR});
        }
    }
}

void ResourceSystem::loadImages(const std::vector<ResourceEntry>& images)
{
    std::unordered_map<glm::uvec2, std::vector<std::pair<std::string, std::shared_ptr<Image>>>> loadedImages;

    mBus.send(LogMessage{"Loading images. " + std::to_string(images.size()) + " images to load.", gResourceName, LogLevel::INFO});
    for(const auto& imageFile : images)
    {
        try
        {
            mBus.send(LogMessage{"Loading " + imageFile.name + ".", gResourceName, LogLevel::VERB});
            std::shared_ptr<Image> image = mCache.access<ImageFromFileLoader>(imageFile.path);

            if(image)
            {
                loadedImages[image->getSize()].push_back({imageFile.name, image});
            }
        }
        catch(ResourceException& exception)
        {
            mBus.send(LogMessage{imageFile.name + ": " + exception.what(), gResourceName, LogLevel::ERR});
        }
    }

    for(const auto& imagesToMakeTextureFrom : loadedImages)
    {
        const glm::uvec2& size = imagesToMakeTextureFrom.first;
        uint32_t amount = imagesToMakeTextureFrom.second.size();
        std::vector<uint8_t> pixels;
        uint32_t newId = mNextTextureId++;
        uint32_t index = 0;

        for(const auto& imageEntry : imagesToMakeTextureFrom.second)
        {
            const auto name = imageEntry.first;
            const auto image = imageEntry.second;
            pixels.insert(pixels.end(), image->getPixelsPointer(), image->getPixelsPointer() + image->getSize().x * image->getSize().y * 4);

            TextureDefinition textureDefinition{newId, index};

            mTextureDefinitions.emplace(mTextureIDs.getId(name), textureDefinition);
            index++;
        }

        std::shared_ptr<TextureArray> textureArray = std::make_shared<TextureArray>();
        textureArray->create(size, amount, pixels.data());

        mBus.send(ResourceDeliverMessage<TextureArray>{newId, textureArray});
    }
}

void ResourceSystem::loadExtensionMetadata(const std::vector<ResourceEntry>& extensionMetadata)
{
    mBus.send(LogMessage{"Loading extension metadata files. " + std::to_string(extensionMetadata.size()) + " files to load.", gResourceName, LogLevel::INFO});
    for(const auto& metadataFile : extensionMetadata)
    {
        try
        {
            mBus.send(LogMessage{"Loading " + metadataFile.name + ".", gResourceName, LogLevel::VERB});
            std::shared_ptr<ExtensionMetadata> metadata = mCache.access<ExtensionMetadataFromFileLoader>(metadataFile.path);

            if(metadata)
            {
                uint32_t id = mExtensionMetadataIDs.getId(metadataFile.name);
                mBus.send(ResourceDeliverMessage<ExtensionMetadata>{id, metadata});
            }
        }
        catch(ResourceException& exception)
        {
            mBus.send(LogMessage{metadataFile.name + ": " + exception.what(), gResourceName, LogLevel::ERR});
        }
    }
}

void ResourceSystem::loadWorlds(const std::vector<ResourceEntry>& worlds)
{
    mBus.send(LogMessage{"Loading world files. " + std::to_string(worlds.size()) + " files to load.", gResourceName, LogLevel::INFO});
    for(const auto& worldFile : worlds)
    {
        try
        {
            mBus.send(LogMessage{"Loading " + worldFile.name + ".", gResourceName, LogLevel::VERB});
            std::shared_ptr<std::vector<WorldParameters>> world = mCache.access<WorldFromFileLoader>(worldFile.path);

            if(world)
            {
                uint32_t id = mExtensionMetadataIDs.getId(worldFile.name);
                mBus.send(ResourceDeliverMessage<std::vector<WorldParameters>>{id, world});
            }
        }
        catch(ResourceException& exception)
        {
            mBus.send(LogMessage{worldFile.name + ": " + exception.what(), gResourceName, LogLevel::ERR});
        }
    }
}
