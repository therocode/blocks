#pragma once
#include <string>
#include "entitydefinition.h"

class EntityDefinitionLoader
{
    EntityDefinition loadFromJSONFile(const std::string& path);
};
