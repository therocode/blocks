#include "scopeddirectory.hpp"
#include "../src/utilities/directorycreator.hpp"

ScopedDirectory::ScopedDirectory(const std::string& path) :
    mPath(path)
{
    bool succeeded = DirectoryCreator::createDirectory(mPath);

    if(!succeeded)
        throw DirectoryCreationException(mPath);
}

ScopedDirectory::~ScopedDirectory()
{
    DirectoryCreator::removeDirectory(mPath, true);
}
