#pragma once
#include <cstdint>
#include <vector>

class Mesh
{
    public:
        Mesh(const std::vector<uint32_t>& indices);
        const std::vector<uint32_t>& getIndices() const;
    private:
        std::vector<uint32_t> mIndices;
};
