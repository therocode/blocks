#include "folderexploder.hpp"
#include "tinydir.hpp"
#include <regex>
#include <iostream>
#include <iostream>

void FolderExploder::explodeFolder(const std::string& directory, const std::vector<std::string>& types, std::vector<std::string>& result)
{
    std::string regex;

    //".*\\.iqm|\
    //.*\\.vert|\
    //.*\\.frag|\
    //.*\\.shad|\
    //.*\\.png"
    
    for(int32_t i = 0; i < types.size(); i++)
    {
        regex += ".*\\." + types[i] + (i != types.size() ? "|" : "");
    }

    std::regex rx(regex);
    
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
            std::cout << "investigating " << fileName << "\n";

            if(regex_match(fileName, rx))
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
