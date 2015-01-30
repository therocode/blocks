#include "resourceexception.hpp"

ResourceException::ResourceException() : runtime_error("Undefined resource exception")
{
}

ResourceException::ResourceException(const std::string& message) : runtime_error(message)
{
}
