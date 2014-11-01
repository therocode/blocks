#include "highlightmanager.hpp"
#include "../utilities/distancechecker.hpp"

ChunkHighlightList HighlightManager::addHighlightEntity(fea::EntityId id, const ChunkCoord& coordinate, uint32_t radius)
{
    EntityMap::const_iterator got = mEntityMap.find(id);
    if(got != mEntityMap.end())
    {
        throw HighlightManagerException("Entity already spawned.");
    }

    mEntityMap[id] = coordinate;
    mRadii[id] = radius;
    
    return highlightShape(coordinate, radius); 
}

ChunkDehighlightList HighlightManager::removeHighlightEntity(fea::EntityId id)
{
    EntityMap::const_iterator got = mEntityMap.find(id);
    if(got == mEntityMap.end())
    {
        throw HighlightManagerException("Entity hasn't spawned.");
    }

    ChunkDehighlightList dehighlightList = dehighlightShape(mEntityMap[id], mRadii[id]);
    mEntityMap.erase(got);
    mRadii.erase(id);

    return dehighlightList;
}

std::pair<ChunkHighlightList, ChunkDehighlightList> HighlightManager::moveHighlightEntity(fea::EntityId id, const ChunkCoord& coordinate)
{
    EntityMap::const_iterator got = mEntityMap.find(id);
    if(got == mEntityMap.end())
    {
        throw HighlightManagerException("Entity hasn't spawned.");
    }

    uint32_t radius = mRadii[id];

    ChunkHighlightList highlightList = highlightShape(coordinate, radius);
    ChunkDehighlightList dehighlightList = dehighlightShape(mEntityMap[id], radius);
    mEntityMap[id] = coordinate;
    return {highlightList, dehighlightList};
}

bool HighlightManager::chunkIsHighlighted(const ChunkCoord& coordinate)
{
    return mRefCounts.count(coordinate) != 0;
}

ChunkHighlightList HighlightManager::highlightShape(const ChunkCoord& loc, uint32_t radius)
{
    ChunkHighlightList highlightList;

    for(int64_t x = loc.x - radius; x < loc.x + radius + 1; ++x)
    {
        for(int64_t y = loc.y - radius; y < loc.y + radius + 1; ++y)
        {
            for(int64_t z = loc.z - radius; z < loc.z + radius + 1; ++z)
            {
                ChunkCoord subLoc(x, y, z);

                if(DistanceChecker::isWithinDistanceOf(loc, subLoc, radius))
                {
                    if(highlightChunk(subLoc))
                    {
                        highlightList.push_back(subLoc);
                    }
                }
            }
        }
    }

    return highlightList;
}

ChunkDehighlightList HighlightManager::dehighlightShape(const ChunkCoord& loc, uint32_t radius)
{
    ChunkDehighlightList dehighlightList;

    for(int64_t x = loc.x - radius; x < loc.x + radius + 1; ++x)
    {
        for(int64_t y = loc.y - radius; y < loc.y + radius + 1; ++y)
        {
            for(int64_t z = loc.z - radius; z < loc.z + radius + 1; ++z)
            {
                ChunkCoord subLoc(x, y, z);

                if(DistanceChecker::isWithinDistanceOf(loc, subLoc, radius))
                {
                    if(dehighlightChunk(subLoc))
                    {
                        dehighlightList.push_back(subLoc);
                    }
                }
            }
        }
    }

    return dehighlightList;
}

bool HighlightManager::highlightChunk(const ChunkCoord& coord)
{
    RefMap::const_iterator got = mRefCounts.find(coord);

    if(got == mRefCounts.end())
    {
        mRefCounts[coord] = 0;
    }

    ++mRefCounts[coord];

    if(mRefCounts[coord] == 1) 
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool HighlightManager::dehighlightChunk(const ChunkCoord& coord)
{
    RefMap::const_iterator got = mRefCounts.find(coord);

    if(got == mRefCounts.end())
    {
        throw HighlightManagerException("Chunk has not been highlighted");
    }

    if(mRefCounts[coord] == 0) 
    {
        throw HighlightManagerException("Chunk has not been highlighted");
    }

    --mRefCounts[coord];

    if(mRefCounts[coord] == 0)
    {
        mRefCounts.erase(coord);
        return true;
    }
    else
    {
        return false;
    }
}
