#pragma once
#include <string>
#include "shaderdefinition.hpp"

class ShaderDefinitionFromFileLoader
{
    public:
        ShaderDefinition load(const std::string& filename);
};
