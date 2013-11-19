#include "package.h"

BasePackage::BasePackage() : mType(PackageType::BASE)
{
}

BasePackage::BasePackage(PackageType type, bool unreliable, int channel) : mType(type), mUnreliable(unreliable), mChannel(channel)
{
}
