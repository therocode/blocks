#pragma once
#include <unordered_set>
#include <unordered_map>
#include "../utilities/glmhash.hpp"
#include "../world/worlddefines.hpp"

class ChunkRequestHandler
{
    public:
        void addRequest(uint32_t id, const ChunkCoord& coordinate);
        uint32_t getRequestAmount(uint32_t id) const;
        uint32_t getTotalRequestAmount() const;
    private:
        std::unordered_map<uint32_t, std::unordered_set<ChunkCoord>> mRequests;
};
