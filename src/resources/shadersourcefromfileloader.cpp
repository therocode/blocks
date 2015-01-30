#include "shadersourcefromfileloader.hpp"
#include <fstream>
#include "resourceexception.hpp"

ShaderSource ShaderSourceFromFileLoader::load(const std::string& filename)
{
    ShaderSource result;

    std::string fileType = filename.substr(filename.find_last_of('.') + 1, std::string::npos);
    std::ifstream file(filename);

    if(file.fail())
    {
        throw ResourceException("Cannot open file " + filename);
    }

    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    result.source = source;

    if(fileType == "vert")
        result.type = ShaderSource::VERTEX;
    else if(fileType == "frag")
        result.type = ShaderSource::FRAGMENT;

    return result;
}
