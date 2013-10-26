#pragma once
#include <string>
#include "entitydefinition.h"

class EntityDefinitionLoader
{
    public:
        EntityDefinition loadFromJSONFile(const std::string& path);
        bool errorOccurred();
        const std::string& getErrorString();
    private:
        std::string mErrorString;
        bool mError;
};
