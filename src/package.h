#pragma once
#include <vector>
#include <stdint.h>

class Package
{
    public:
        virtual std::vector<uint8_t> serialise() const = 0;
        virtual void deserialise(std::vector<uint8_t> bytes) = 0;
};
