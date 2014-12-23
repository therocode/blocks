#include "extensionmetadatafromfileloader.hpp"
#include <json/json.hpp>
#include <fstream>
#include <iostream>

ExtensionMetadata ExtensionMetadataFromFileLoader::load(const std::string& filename)
{
    ExtensionMetadata metadata;

    std::fstream file(filename);

    if(file.fail())
    {
        std::cerr << "ERROR: Cannot open extension metadata file " << filename << "..." << std::endl;
        exit(0); // TODO: Should be exception
    }

    json::Value root;
    root.SetObject();

    json::read(file, root);

    metadata.setDescription(root.GetStringMember("description"));
    metadata.setVersion(root.GetStringMember("version"));

    return metadata;
}
