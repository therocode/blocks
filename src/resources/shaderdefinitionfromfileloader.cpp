#include "shaderdefinitionfromfileloader.hpp"
#include <json/json.hpp>
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

    json::Value root;
    root.SetObject();
    
    json::read(file, root);

    definition.name = root.GetStringMember("name");
    definition.vertexShader = root.GetStringMember("vertex");
    definition.fragmentShader = root.GetStringMember("fragment");

    return definition;
}
