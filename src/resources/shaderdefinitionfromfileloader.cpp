#include "shaderdefinitionfromfileloader.hpp"
#include <json/reader.h>
#include <json/value.h>
#include <fstream>
#include <iostream>

ShaderDefinition ShaderDefinitionFromFileLoader::load(const std::string& filename)
{
    ShaderDefinition definition;

    std::fstream file(filename);

    if(file.fail())
    {
        std::cout << "ERROR: Cannot open the file...\n";
        exit(0); //should be exception
    }

    Json::Value root;
    Json::Reader reader;
    
    reader.parse(file, root, false);

    const Json::Value& vertexNames = root["vertex_shaders"];

    if(vertexNames.isArray())
    {
        for(const auto& vertexName : vertexNames)
        {
            definition.vertexModules.push_back(vertexName.asString());
        }
    }

    const Json::Value& fragmentNames = root["fragment_shaders"];

    if(fragmentNames.isArray())
    {
        for(const auto& fragmentName : fragmentNames)
        {
            definition.fragmentModules.push_back(fragmentName.asString());
        }
    }

    return definition;
}
