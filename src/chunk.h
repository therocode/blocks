#pragma once
#include <stdint.h>
#include <functional>

class ChunkCoordinate
{
    public:
        bool operator==(const ChunkCoordinate& other) const;
        uint32_t x;
        uint32_t y;
        uint32_t z;
};

class Chunk
{
    private:
};

namespace std
{
    template<>
        struct std::hash<ChunkCoordinate>
        {
            public:
                std::size_t operator()(ChunkCoordinate const& coord) const 
                {
                    return coord.x ^ (coord.y ^ (coord.z << 1) << 1);
                }
        };
}
