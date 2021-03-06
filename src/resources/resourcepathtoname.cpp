#include "resourcepathtoname.hpp"
#include <fea/assert.hpp>

std::string resourcePathToName(const std::string& rootPath, const std::string& path)
{
    FEA_ASSERT(path.find_first_of(rootPath) == 0, "Error! Path must start with root path.");

    std::string result = path.substr(rootPath.size() + 1, std::string::npos);

    std::string extensionName = result.substr(0, result.find_first_of('/'));
    std::string fileNameAndType = result.substr(result.find_last_of('/') + 1, std::string::npos);
    std::string fileType = fileNameAndType.substr(fileNameAndType.find_last_of('.') + 1, std::string::npos);
    std::string fileName = fileNameAndType.substr(0, fileNameAndType.size() - fileType.size() - 1);

    std::string category;

    if(fileType == "iqm")
        category = "models";
    else if(fileType == "vert")
        category = "vertex_shaders";
    else if(fileType == "frag")
        category = "fragment_shaders";
    else if(fileType == "shad")
        category = "shaders";
    else if(fileType == "png")
        category = "textures";
    else if(fileType == "meta")
        category = "metadata";
	else if(fileType == "wld")
        category = "world";
	else if(fileType == "gfe")
        category = "gfx_entity";
	else if(fileType == "att")
        category = "attribute";
	else if(fileType == "ent")
        category = "entity";
	else if(fileType == "ttf")
        category = "font";

    result = extensionName + "." + category + "." + fileName;

    return result;
}
