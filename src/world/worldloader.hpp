#pragma once
#include <vector>
#include <string>
#include "worldparameters.hpp"

class WorldLoader
{
    public:
        WorldLoader();
        void loadWorldFile(const std::string& path);
        const std::vector<WorldParameters> getLoadedWorlds() const;
        bool hasError() const;
        const std::string& getErrorString() const;
    private:
        IntRange rangeParser(const std::string& rangeString);
        bool isNumber(const std::string& s);
        std::vector<WorldParameters> mLoadedWorlds;
        std::string mErrorString;
        bool mError;
};
