#include "entitydefinitionloader.h"
#include "controllers/physicstype.h"
#include "../json/json.h"
#include <fstream>

EntityDefinition EntityDefinitionLoader::loadFromJSONFile(const std::string& path)
{
    mError = false;
    EntityDefinition definition;

    //setting defaults
    definition.category = "living";
    definition.physics = PhysicsType::FALLING;

    std::ifstream file(path);

    if(!file)
    {
        mErrorString = "Error, file '" + path + "' could not be found!";
        mError = true;
        return definition;
    }

    json::Value root;
    root.SetObject();
    json::read(file, root);

    json::Member entity = root.GetMember(0);
    //reading name
    definition.name = entity.name;

    json::Value entityMembers = entity.value;
    std::size_t attributeAmount = entityMembers.GetNumMembers();
    
    for(uint32_t i = 0; i < attributeAmount; i++)
    {
        //reading category
        json::Member member = entityMembers.GetMember(i);
        std::string memberName = member.name;

        if(memberName == "category")
            definition.category = member.value.GetString();
        
        //reading physics type
        else if(memberName == "physics_type")
        {
            std::string physicsString = member.value.GetString();

            if(physicsString == "FALLING")
                definition.physics = PhysicsType::FALLING;
            else if(physicsString == "FLOATING")
                definition.physics = PhysicsType::FLOATING;
            else
            {
                mErrorString = "Error in file '" + path + "': physics_type " + physicsString + " is not a valid type! (Try FALLING or FLOATING)";
                mError = true;
                return definition;
            }
        }
    }

    return definition;
}

bool EntityDefinitionLoader::errorOccurred()
{
    return mError;
}

const std::string& EntityDefinitionLoader::getErrorString()
{
    return mErrorString;
}
