#include "extensionmetadatafromfileloader.hpp"
#include <json/json.hpp>
#include <fstream>
#include "resourceexception.hpp"

ExtensionMetadata ExtensionMetadataFromFileLoader::load(const std::string& filename)
{
    ExtensionMetadata metadata;

    std::fstream file(filename);

    if(file.fail())
    {
        throw ResourceException("Cannot open file " + filename);
    }

    json::Value root;
    root.SetObject();

    json::read(file, root);

    metadata.setDescription(root.GetStringMember("description"));
    metadata.setVersion(root.GetStringMember("version"));

    return metadata;
}
