#pragma once
#include <vector>
#include <string>

//regex example:    
//exploder.explodeFolder("data", ".*\\.as|.*\\.def", sourceFiles);

class FolderExploder
{
    public:
        void explodeFolder(const std::string& directory, const std::vector<std::string>& types, std::vector<std::string>& result);
        std::vector<std::string> getSubFolders(const std::string& directory);
};
