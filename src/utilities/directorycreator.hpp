#pragma once
#include <string>

class DirectoryCreator
{
    public:
        static bool directoryExists(const std::string& path);
        static bool createDirectory(const std::string& path);
        static bool removeDirectory(const std::string& path, bool force = false);
};
