#pragma once
#include "package.h"
#include "chunk.h"

class GfxEntityMovedPackage : public Package
{
    public:
        GfxEntityMovedPackage(size_t id, const glm::vec3& position);
        virtual std::vector<uint8_t> serialise() const;
        virtual void deserialise(std::vector<uint8_t> bytes);
        size_t mId;
        glm::vec3 mPosition;
};
