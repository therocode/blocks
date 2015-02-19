#include "gfxentitydefinitionfromfileloader.hpp"
#include <json/json.hpp>
#include <fstream>
#include "resourceexception.hpp"

GfxEntityDefinitionFromFileLoader::GfxEntityDefinitionFromFileLoader(const IdMapper<std::string>& textureIds, const IdMapper<std::string>& modelIds) : 
    mTextureIds(textureIds),
    mModelIds(modelIds)
{
}

GfxEntityDefinition GfxEntityDefinitionFromFileLoader::load(const std::string& filename)
{
    GfxEntityDefinition gfxEntityDefinition;

    std::fstream file(filename);

    if(file.fail())
    {
        throw ResourceException("Cannot open file " + filename);
    }

    json::Value root;
    root.SetObject();

    json::read(file, root);

    if(root.HasMember("texture"))
    {
        std::string textureName = root.GetStringMember("texture");

        if(mTextureIds.valueExists(textureName))
        {
            gfxEntityDefinition.textureId = mTextureIds.getIdConst(textureName);
        }
        else
        {
            throw ResourceException("Given texture '" + textureName + "' does not exist");
        }
    }
    else
    {
        throw ResourceException("No texture given");
    }

    if(root.HasMember("model"))
    {
        std::string modelName = root.GetStringMember("model");

        if(mModelIds.valueExists(modelName))
        {
            gfxEntityDefinition.modelId = mModelIds.getIdConst(modelName);
        }
        else
        {
            throw ResourceException("Given model '" + modelName + "' does not exist");
        }
    }
    else
    {
        throw ResourceException("No model given");
    }

    return gfxEntityDefinition;
}
