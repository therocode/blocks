#pragma once
#include "shadersource.hpp"

class ShaderSourceFromFileLoader
{
    public:
        ShaderSource load(const std::string& filename);
};
