#include "chunkrequesthandler.hpp"

void ChunkRequestHandler::addRequest(uint32_t id, const ChunkCoord& coordinate)
{
    mRequests[id].insert(coordinate);   
}

uint32_t ChunkRequestHandler::getRequestAmount(uint32_t id) const
{
    const auto& iterator = mRequests.find(id);

    return iterator != mRequests.end() ? iterator->second.size() : 0;
}

uint32_t ChunkRequestHandler::getTotalRequestAmount() const
{
    uint32_t total = 0;

    for(const auto& requestList : mRequests)
        total += requestList.second.size();

    return total;
}
