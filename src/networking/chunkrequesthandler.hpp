#pragma once
#include <unordered_set>
#include <unordered_map>
#include "../utilities/glmhash.hpp"
#include "../world/worlddefines.hpp"

class ChunkRequestHandler
{
    public:
        void addRequest(uint32_t id, const ChunkCoord& coordinate);
        void clearRequestsFor(uint32_t id);
        uint32_t getRequesterAmount(const ChunkCoord& coordinate) const;
        uint32_t getTotalRequesterAmount() const;
        std::unordered_set<uint32_t> getAndRemove(const ChunkCoord& coordinate);
    private:
        std::unordered_map<ChunkCoord, std::unordered_set<uint32_t>> mRequests;
};
