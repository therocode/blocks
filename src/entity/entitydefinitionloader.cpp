#include "entitydefinitionloader.h"
#include "../json/json.h"
#include <fstream>

EntityDefinition EntityDefinitionLoader::loadFromJSONFile(const std::string& path)
{
    EntityDefinition definition;

    std::ifstream file(path);

    if(!file)
    {
        std::cout << "Error, file '" << path << "' could not be found!\n";
        exit(4);
    }

    json::Value root;
    root.SetObject();
    json::read(file, root);

    json::Member temp = root.GetMember(0);
    //reading name
    definition.name = temp.name;
    
    //reading category
    json::Member category = temp.value.GetMember(0);
    definition.category = category.value.GetString();

    return definition;
}
