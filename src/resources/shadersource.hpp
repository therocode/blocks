#pragma once
#include <string>

struct ShaderSource
{
    enum { VERTEX, FRAGMENT };
    std::string name;
    std::string source;
    int32_t type;
};
