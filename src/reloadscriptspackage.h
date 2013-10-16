#pragma once
#include "package.h"
#include "chunk.h"

class ReloadScriptsPackage : public Package
{
    public:
        ReloadScriptsPackage();
        virtual std::vector<uint8_t> serialise() const;
        virtual void deserialise(std::vector<uint8_t> bytes);
};
