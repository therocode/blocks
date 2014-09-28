#include "directorycreator.hpp"
#include <fea/assert.hpp>

#if defined(__linux)

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

bool DirectoryCreator::directoryExists(const std::string& path)
{
    struct stat st = {0};

    return stat(path.c_str(), &st) != -1 ? true : false;
}

bool DirectoryCreator::createDirectory(const std::string& path)
{
    return mkdir(path.c_str(), 0755) != -1 ? true : false;
}

bool DirectoryCreator::removeDirectory(const std::string& path)
{
    return rmdir(path.c_str()) != -1 ? true : false;
}

#elif defined(__WIN32__)

#include <windows.h>

bool DirectoryCreator::directoryExists(const std::string& path)
{
    return GetFileAttributes(path.c_str()) == FILE_ATTRIBUTE_DIRECTORY ? true : false;
}

bool DirectoryCreator::createDirectory(const std::string& path)
{
    return CreateDirectory(path.c_str(), NULL) != 0 ? true : false;
}

bool DirectoryCreator::removeDirectory(const std::string& path)
{
    return RemoveDirectory(path.c_str());
}

#else

//if none of the above implementations apply for this system
bool DirectoryCreator::directoryExists(const std::string& path)
{
    errorDirectoryCreatorNotImplementedOnThisPlatform; //intentional compilation error
    return false;
}

bool DirectoryCreator::createDirectory(const std::string& path)
{
    errorDirectoryCreatorNotImplementedOnThisPlatform; //intentional compilation error
    return false;
}

bool DirectoryCreator::removeDirectory(const std::string& path)
{
    errorDirectoryCreatorNotImplementedOnThisPlatform; //intentional compilation error
    return false;
}

#endif
