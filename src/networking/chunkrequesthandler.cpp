#include "chunkrequesthandler.hpp"

void ChunkRequestHandler::addRequest(uint32_t id, const ChunkCoord& coordinate)
{
    mRequests[coordinate].insert(id);   
}

void ChunkRequestHandler::clearRequestsFor(uint32_t id)
{
    for(auto& requestList : mRequests)
        requestList.second.erase(id);
}

uint32_t ChunkRequestHandler::getRequesterAmount(const ChunkCoord& coordinate) const
{
    const auto& iterator = mRequests.find(coordinate);

    return iterator != mRequests.end() ? iterator->second.size() : 0;
}

uint32_t ChunkRequestHandler::getTotalRequesterAmount() const
{
    uint32_t total = 0;

    for(const auto& requestList : mRequests)
        total += requestList.second.size();

    return total;
}

std::unordered_set<uint32_t> ChunkRequestHandler::getAndRemove(const ChunkCoord& coordinate)
{
    std::unordered_set<uint32_t> result;
    const auto& iterator = mRequests.find(coordinate);

    if(iterator != mRequests.end())
    {
        result = std::move(iterator->second);

        mRequests.erase(iterator);
    }

    return result;
}
