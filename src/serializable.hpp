#pragma once
#include <cstdint>
#include <vector>

class Serializable
{
    virtual std::vector<uint8_t> toBytes() const = 0;
    virtual void fromBytes(const std::vector<uint8_t>& bytes) = 0;
};
