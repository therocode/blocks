#pragma once
#include <string>
#include "rawmodel.hpp"

class IQMFromFileLoader
{
    public:
        RawModel load(const std::string& filename);
};
