#include "fontfromfileloader.hpp"
#include <json/json.hpp>
#include <fstream>
#include "resourceexception.hpp"

gim::Font FontFromFileLoader::load(const std::string& filename)
{
    std::fstream fileStream(filename);

    auto toReturn = gim::Font(fileStream);
    return toReturn;
}
