#pragma once
#include <unordered_set>
#include <unordered_map>
#include "../utilities/glmhash.hpp"
#include "../world/worlddefines.hpp"

class ChunkRequestHandler
{
    public:
        void addRequester(const ChunkCoord& coordinate, uint32_t id);
        uint32_t getRequesterAmount(const ChunkCoord& coordinate) const;
        uint32_t getTotalRequesterAmount() const;
    private:
        std::unordered_map<ChunkCoord, std::unordered_set<uint32_t>> mRequests;
};
