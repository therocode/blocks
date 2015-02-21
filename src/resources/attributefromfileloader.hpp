#pragma once
#include <string>
#include <unordered_map>

class AttributeFromFileLoader
{
    public:
        std::unordered_map<std::string, std::string> load(const std::string& filename);
};
