#include "folderexploder.hpp"
#include "tinydir.hpp"

void FolderExploder::explodeFolder(const std::string& directory, const std::vector<std::string>& types, std::vector<std::string>& result)
{
    std::string regex;

    tinydir_dir dir;

    tinydir_open(&dir, directory.c_str());

    while(dir.has_next)
    {
        tinydir_file file;
        tinydir_readfile(&dir, &file);

        const std::string fileName = file.name;

        if(file.is_dir)
        {
            if(fileName[0] != '.')
            {
                explodeFolder(directory + "/" + fileName, types, result);
            }
        }
        else
        {
            bool found = false;

            for(const auto& type : types)
            {
                //fileName.find_la
                if(fileName.find(std::string(".") + type) == fileName.size() - type.size() - 1)
                {
                    found = true;
                    break;
                }
            }

            if(found)
            {
                result.push_back(directory + "/" + fileName);
            }
        }

        tinydir_next(&dir);
    }
}

std::vector<std::string> FolderExploder::getSubFolders(const std::string& directory)
{
    std::vector<std::string> result;
    tinydir_dir dir;

    tinydir_open(&dir, directory.c_str());

    while(dir.has_next)
    {
        tinydir_file file;
        tinydir_readfile(&dir, &file);

        std::string fileName = file.name;

        if(file.is_dir)
        {
            if(fileName[0] != '.')
            {
                result.push_back(fileName);
            }
        }

        tinydir_next(&dir);
    }

    return result;
}
