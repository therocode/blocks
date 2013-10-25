#pragma once
#include <string>
#include "entitydefinition.h"

class EntityDefinitionLoader
{
    public:
        EntityDefinition loadFromJSONFile(const std::string& path);
};
