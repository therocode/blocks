#pragma once
#include <string>

class ExtensionMetadata
{
    public:
        std::string getDescription();
        void setDescription(std::string description);
        std::string getVersion();
        void setVersion(std::string version);
    
    private:
        std::string mDescription;
        std::string mVersion;
};
