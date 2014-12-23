#pragma once
#include <string>
#include "extensionmetadata.hpp"

class ExtensionMetadataFromFileLoader
{
    public:
        ExtensionMetadata load(const std::string& filename);
};
