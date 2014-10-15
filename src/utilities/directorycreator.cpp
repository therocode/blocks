#include "directorycreator.hpp"
#include <fea/assert.hpp>

#if defined(__linux)

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <cstring>

int remove_directory_force(const char *path)
{
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;

    if (d)
    {
        struct dirent *p;

        r = 0;

        while (!r && (p=readdir(d)))
        {
            int r2 = -1;
            char *buf;
            size_t len;

            /* Skip the names "." and ".." as we don't want to recurse on them. */
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
            {
                continue;
            }

            len = path_len + strlen(p->d_name) + 2; 
            buf = (char*) malloc(len);

            if (buf)
            {
                struct stat statbuf;

                snprintf(buf, len, "%s/%s", path, p->d_name);

                if (!stat(buf, &statbuf))
                {
                    if (S_ISDIR(statbuf.st_mode))
                    {
                        r2 = remove_directory_force(buf);
                    }
                    else
                    {
                        r2 = unlink(buf);
                    }
                }

                free(buf);
            }

            r = r2;
        }

        closedir(d);
    }

    if (!r)
    {
        r = rmdir(path);
    }

    return r;
}

bool DirectoryCreator::directoryExists(const std::string& path)
{
    struct stat st = {0};

    return stat(path.c_str(), &st) != -1 ? true : false;
}

bool DirectoryCreator::createDirectory(const std::string& path)
{
    return mkdir(path.c_str(), 0755) != -1 ? true : false;
}

bool DirectoryCreator::removeDirectory(const std::string& path, bool force)
{
    if(!force)
        return rmdir(path.c_str()) != -1 ? true : false;
    else
        return !remove_directory_force(path.c_str());
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

bool DirectoryCreator::removeDirectory(const std::string& path, bool force)
{
	if(!force)
	{
		return RemoveDirectory(path.c_str());
	}
	else
	{
		SHFILEOPSTRUCT file_op = {
			NULL,
			FO_DELETE,
			(path + '\0').c_str(),
			"",
			FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT,
			false,
			0,
			""};
		
		return !SHFileOperation(&file_op);
	}
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
