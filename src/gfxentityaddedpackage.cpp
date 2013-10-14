#include "gfxentityaddedpackage.h"

GfxEntityAddedPackage::GfxEntityAddedPackage(size_t id, const glm::vec3& position) : Package(typeid(GfxEntityAddedPackage)),
                                                                           mId(id),
                                                                           mPosition(position)
{
}

std::vector<uint8_t> GfxEntityAddedPackage::serialise() const
{
    //todo
}

void GfxEntityAddedPackage::deserialise(std::vector<uint8_t> bytes)
{
    //todo
}
