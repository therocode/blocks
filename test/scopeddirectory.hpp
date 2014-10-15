#pragma once
#include <string>
#include <stdexcept>

class DirectoryCreationException : public std::runtime_error
{
        public:
            DirectoryCreationException(const std::string& path) :
                std::runtime_error("Error creating scoped directory at '" + path + "'")
    {   
    }   
};

class ScopedDirectory
{
    public:
        ScopedDirectory(const std::string& path);
        ~ScopedDirectory();
        ScopedDirectory(const ScopedDirectory&) = delete;
        ScopedDirectory& operator=(const ScopedDirectory&) = delete;
    private:
        const std::string mPath;
};
