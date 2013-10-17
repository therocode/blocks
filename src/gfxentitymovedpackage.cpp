#include "gfxentitymovedpackage.h"

GfxEntityMovedPackage::GfxEntityMovedPackage(size_t id, const glm::vec3& position) : BasePackage(typeid(GfxEntityMovedPackage)),
                                                                           mId(id),
                                                                           mPosition(position)
{
}

std::vector<uint8_t> GfxEntityMovedPackage::serialise() const
{
    //todo
}

void GfxEntityMovedPackage::deserialise(std::vector<uint8_t> bytes)
{
    //todo
}
