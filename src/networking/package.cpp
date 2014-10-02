#include "package.hpp"

BasePackage::BasePackage() : mType(PackageType::BASE)
{
}

BasePackage::BasePackage(PackageType type) : mType(type)
{
}
