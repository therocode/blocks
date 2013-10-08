#include <stdint.h>

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
