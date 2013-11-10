#include "package.h"

BasePackage::BasePackage() : mType(PackageType::BASE)
{
}

BasePackage::BasePackage(PackageType type) : mType(type)
{
}
