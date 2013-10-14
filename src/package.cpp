#include "package.h"

Package::Package() : mType(typeid(Package))
{
}

Package::Package(std::type_index type) : mType(type)
{
}
