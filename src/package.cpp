#include "package.h"

BasePackage::BasePackage() : mType(typeid(BasePackage))
{
}

BasePackage::BasePackage(std::type_index type) : mType(type)
{
}
