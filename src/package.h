#pragma once
#include <vector>
#include <stdint.h>
#include <typeindex>

class Package
{
    public:
        Package();
        Package(std::type_index type);
        virtual std::vector<uint8_t> serialise() const = 0;
        virtual void deserialise(std::vector<uint8_t> bytes) = 0;
        std::type_index mType;
};
