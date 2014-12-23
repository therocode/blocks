#include "extensionmetadata.hpp"

std::string ExtensionMetadata::getDescription()
{
    return mDescription;
}

void ExtensionMetadata::setDescription(std::string description)
{
    mDescription = description;
}

std::string ExtensionMetadata::getVersion()
{
    return mVersion;
}

void ExtensionMetadata::setVersion(std::string version)
{
    mVersion = version;
}
