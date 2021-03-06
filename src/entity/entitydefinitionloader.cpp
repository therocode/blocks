#include "entitydefinitionloader.hpp"
#include "controllers/physicstype.hpp"
#include <json/json.hpp>
#include <fstream>

EntityDefinition EntityDefinitionLoader::loadFromJSONFile(const std::string& path)
{
    mError = false;
    EntityDefinition definition;

    //setting defaults
    definition.category = "living";
    definition.physicsType = PhysicsType::FALLING;
    definition.drag = 1.0f;
    definition.walkSpeed = 0.0f;
    definition.runSpeed = 0.0f;

    std::ifstream file(path);

    if(!file)
    {
        mErrorString = "Error, entity file '" + path + "' could not be found!";
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
                definition.physicsType = PhysicsType::FALLING;
            else if(physicsString == "FLOATING")
                definition.physicsType = PhysicsType::FLOATING;
            else
            {
                mErrorString = "Error in file '" + path + "': physics_type " + physicsString + " is not a valid type! (Try FALLING or FLOATING)";
                mError = true;
                return definition;
            }
        }
        //reading drag
        else if(memberName == "drag")
        {
            definition.drag = member.value.GetFloat();
        }
        //reading walkspeed
        else if(memberName == "walk_speed")
        {
            definition.walkSpeed = member.value.GetFloat();
        }
        //reading runspeed
        else if(memberName == "run_speed")
        {
            definition.runSpeed = member.value.GetFloat();
        }
        //reading jumstrength
        else if(memberName == "jump_strength")
        {
            definition.jumpStrength = member.value.GetFloat();
        }
        //reading gfx_entity
        else if(memberName == "gfx_entity")
        {
            definition.gfxEntity = member.value.GetString();
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
