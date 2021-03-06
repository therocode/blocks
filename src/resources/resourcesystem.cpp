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
#include "attributefromfileloader.hpp"
#include "entityfromfileloader.hpp"
#include "resourceexception.hpp"
#include "gfxentitydefinition.hpp"
#include "gfxentitydefinitionfromfileloader.hpp"
#include "../utilities/gimgui.hpp"
#include "fontfromfileloader.hpp"

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
        if(std::find(fileTypes.begin(), fileTypes.end(), "gfe") != fileTypes.end())
            loadGfxEntities(mResourceList["gfe"]);
        if(std::find(fileTypes.begin(), fileTypes.end(), "att") != fileTypes.end())
            loadAttributes(mResourceList["att"]);
        if(std::find(fileTypes.begin(), fileTypes.end(), "ent") != fileTypes.end())
            loadEntities(mResourceList["ent"]);
        if(std::find(fileTypes.begin(), fileTypes.end(), "ttf") != fileTypes.end())
            loadFonts(mResourceList["ttf"]);
    }
}

uint32_t ResourceSystem::getGfxEntityId(const std::string& gfxEntityName) const
{
    return mGfxEntityIDs.getIdConst(gfxEntityName);
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
                mBus.send(ResourceDeliverMessage<RawModel>{id, modelFile.name, model});
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
                mBus.send(ResourceDeliverMessage<ShaderSource>{id, vertexShaderFile.name, vertexShader});
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
                mBus.send(ResourceDeliverMessage<ShaderSource>{id, fragmentShaderFile.name, fragmentShader});
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
                mBus.send(ResourceDeliverMessage<ShaderDefinition>{id, shaderDefinitionFile.name, shaderDefinition});
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

            std::shared_ptr<TextureDefinition> textureDefinition = std::make_shared<TextureDefinition>(TextureDefinition{newId, index, image->getSize()});

            mTextureDefinitions.emplace(mTextureIDs.getId(name), textureDefinition);
            mBus.send(ResourceDeliverMessage<TextureDefinition>{mTextureIDs.getId(name), name, textureDefinition});
            index++;
        }

        std::shared_ptr<TextureArray> textureArray = std::make_shared<TextureArray>();
        textureArray->create(size, amount, pixels.data());

        mBus.send(ResourceDeliverMessage<TextureArray>{newId, "textureArray", textureArray});
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
                mBus.send(ResourceDeliverMessage<ExtensionMetadata>{id, metadataFile.name, metadata});
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
                uint32_t id = mWorldIDs.getId(worldFile.name);
                mBus.send(ResourceDeliverMessage<std::vector<WorldParameters>>{id, worldFile.name, world});
            }
        }
        catch(ResourceException& exception)
        {
            mBus.send(LogMessage{worldFile.name + ": " + exception.what(), gResourceName, LogLevel::ERR});
        }
    }
}

void ResourceSystem::loadGfxEntities(const std::vector<ResourceEntry>& gfxEntities)
{
    mBus.send(LogMessage{"Loading graphic entity files. " + std::to_string(gfxEntities.size()) + " files to load.", gResourceName, LogLevel::INFO});
    for(const auto& gfxEntityFile : gfxEntities)
    {
        try
        {
            mBus.send(LogMessage{"Loading " + gfxEntityFile.name + ".", gResourceName, LogLevel::VERB});
            std::shared_ptr<GfxEntityDefinition> gfxEntity = mCache.access<GfxEntityDefinitionFromFileLoader>(gfxEntityFile.path, mTextureIDs, mModelIDs);

            if(gfxEntity)
            {
                uint32_t id = mGfxEntityIDs.getId(gfxEntityFile.name);
                mBus.send(ResourceDeliverMessage<GfxEntityDefinition>{id, gfxEntityFile.name, gfxEntity});
            }
        }
        catch(ResourceException& exception)
        {
            mBus.send(LogMessage{gfxEntityFile.name + ": " + exception.what(), gResourceName, LogLevel::ERR});
        }
    }
}

void ResourceSystem::loadAttributes(const std::vector<ResourceEntry>& attributes)
{
    mBus.send(LogMessage{"Loading attribute files. " + std::to_string(attributes.size()) + " files to load.", gResourceName, LogLevel::INFO});
    for(const auto& attributeFile : attributes)
    {
        try
        {
            mBus.send(LogMessage{"Loading " + attributeFile.name + ".", gResourceName, LogLevel::VERB});
            std::shared_ptr<std::unordered_map<std::string, std::string>> attribute = mCache.access<AttributeFromFileLoader>(attributeFile.path);

            if(attribute)
            {
                uint32_t id = mAttributeIDs.getId(attributeFile.name);
                mBus.send(ResourceDeliverMessage<std::unordered_map<std::string, std::string>>{id, attributeFile.name, attribute});
            }
        }
        catch(ResourceException& exception)
        {
            mBus.send(LogMessage{attributeFile.name + ": " + exception.what(), gResourceName, LogLevel::ERR});
        }
    }
}

void ResourceSystem::loadEntities(const std::vector<ResourceEntry>& entities)
{
    mBus.send(LogMessage{"Loading entity files. " + std::to_string(entities.size()) + " files to load.", gResourceName, LogLevel::INFO});
    for(const auto& entityFile : entities)
    {
        try
        {
            mBus.send(LogMessage{"Loading " + entityFile.name + ".", gResourceName, LogLevel::VERB});
            std::shared_ptr<std::vector<std::pair<std::string, fea::EntityTemplate>>> entity = mCache.access<EntityFromFileLoader>(entityFile.path);

            if(entity)
            {
                uint32_t id = mEntityIDs.getId(entityFile.name);
                mBus.send(ResourceDeliverMessage<std::vector<std::pair<std::string, fea::EntityTemplate>>>{id, entityFile.name, entity});
            }
        }
        catch(ResourceException& exception)
        {
            mBus.send(LogMessage{entityFile.name + ": " + exception.what(), gResourceName, LogLevel::ERR});
        }
    }
}

void ResourceSystem::loadFonts(const std::vector<ResourceEntry>& fonts)
{
    mBus.send(LogMessage{"Loading font files. " + std::to_string(fonts.size()) + " files to load.", gResourceName, LogLevel::INFO});
    for(const auto& fontFile : fonts)
    {
        try
        {
            mBus.send(LogMessage{"Loading " + fontFile.name + ".", gResourceName, LogLevel::VERB});
            std::shared_ptr<gim::Font> font = mCache.access<FontFromFileLoader>(fontFile.path);

            if(font)
            {
                uint32_t id = mFontIDs.getId(fontFile.name);
                mBus.send(ResourceDeliverMessage<gim::Font>{id, fontFile.name, font});
            }
        }
        catch(ResourceException& exception)
        {
            mBus.send(LogMessage{fontFile.name + ": " + exception.what(), gResourceName, LogLevel::ERR});
        }
    }
}
