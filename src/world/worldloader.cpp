#include "worldloader.hpp"
#include <json/json.hpp>
#include <fstream>
#include <limits>

WorldLoader::WorldLoader() :
    mError(false)
{
}

void WorldLoader::loadWorldFile(const std::string& path)
{
    std::ifstream file(path);

    if(!file)
    {
        mErrorString = "Error, world file '" + path + "' could not be found!\n";
        mError = true;
        return;
    }

    json::Value root;
    root.SetObject();
    
    json::read(file, root);

    json::Value worlds = root.GetArrayMember("worlds");

    int32_t worldAmount = worlds.GetNumElements();

    for(int32_t i = 0; i < worldAmount; i++)
    {
        json::Value world = worlds.GetElement(i);
        WorldParameters worldParameters;

        worldParameters.identifier = world.GetStringMember("identifier");
        worldParameters.title = world.GetStringMember("title");
        worldParameters.ranges.xRange = rangeParser(world.GetStringMember("x_range"));
        worldParameters.ranges.yRange = rangeParser(world.GetStringMember("y_range"));
        worldParameters.ranges.zRange = rangeParser(world.GetStringMember("z_range"));

        mLoadedWorlds.push_back(worldParameters);
    }
}

const std::vector<WorldParameters> WorldLoader::getLoadedWorlds() const
{
    return mLoadedWorlds;
}

IntRange WorldLoader::rangeParser(const std::string& rangeString)
{
    IntRange range;

    if(rangeString.find("..") != std::string::npos)
    {
        size_t dotPosition = rangeString.find("..");
        std::string first  = rangeString.substr(0, dotPosition);
        std::string second = rangeString.substr(dotPosition + 2);

        if(first == "MIN")
            range.first = std::numeric_limits<int64_t>::min();
        else if(isNumber(first))
            range.first = std::stoi(first);
        else
        {
            mErrorString += "Error parsing range part: " + first + "\n";
            mError = true;
            return IntRange();
        }

        if(second == "MAX")
            range.second = std::numeric_limits<int64_t>::max();
        else if(isNumber(second))
            range.second = std::stoi(second);
        else
        {
            mErrorString += "Error parsing range part: " + second + "\n";
            mError = true;
            return IntRange();
        }

        return range;
    }
    else
    {
        mErrorString += "Error parsing range string: " + rangeString + "\n";
        mError = true;
        return IntRange();
    }
}

bool WorldLoader::isNumber(const std::string& s)
{
    return !s.empty() && s.find_first_not_of("0123456789-") == std::string::npos;
}

bool WorldLoader::hasError() const
{
    return mError;
}

const std::string& WorldLoader::getErrorString() const
{
    return mErrorString;
}
