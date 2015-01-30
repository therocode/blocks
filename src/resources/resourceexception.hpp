#pragma once
#include <stdexcept>

class ResourceException : public std::runtime_error
{
    public:
        ResourceException();
        ResourceException(const std::string& message);
};
