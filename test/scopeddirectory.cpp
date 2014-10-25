#include "scopeddirectory.hpp"
#include "../src/utilities/directorycreator.hpp"

ScopedDirectory::ScopedDirectory(const std::string& path) :
    mPath(path)
{
    mCreated = DirectoryCreator::createDirectory(mPath);

    if(!mCreated)
        throw DirectoryCreationException(mPath);
}

ScopedDirectory::~ScopedDirectory()
{
    if(mCreated)
    {
        DirectoryCreator::removeDirectory(mPath, true);
    }
}

const std::string& ScopedDirectory::getPath() const
{
    return mPath;
}
