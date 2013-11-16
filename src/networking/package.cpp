#include "package.h"

BasePackage::BasePackage() : mType(PackageType::BASE)
{
}

BasePackage::BasePackage(PackageType type, bool unreliable) : mType(type), mUnreliable(unreliable)
{
}
