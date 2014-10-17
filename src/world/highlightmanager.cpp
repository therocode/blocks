#include "highlightmanager.hpp"

using namespace std;

HighlightManager::HighlightManager(int highlightRadius)
    : mHighlightRadius(highlightRadius) 
{
}


ChunkHighlightList HighlightManager::addHighlightEntity(fea::EntityId id, const ChunkCoord& coordinate)
{
    EntityMap::const_iterator got = mEntityMap.find(id);
    if(got != mEntityMap.end())
    {
        throw HighlightManagerException("Entity already spawned.");
    }

    mEntityMap[id] = coordinate;
    
    return highlightShape(coordinate); 
}

ChunkDehighlightList HighlightManager::removeHighlightEntity(fea::EntityId id)
{
    EntityMap::const_iterator got = mEntityMap.find(id);
    if(got == mEntityMap.end())
    {
        throw HighlightManagerException("Entity hasn't spawned.");
    }

    ChunkDehighlightList dehighlightList = dehighlightShape(mEntityMap[id]);
    mEntityMap.erase(got);

    return dehighlightList;
}

std::pair<ChunkHighlightList, ChunkDehighlightList> HighlightManager::moveHighlightEntity(fea::EntityId id, const ChunkCoord& coordinate)
{
    EntityMap::const_iterator got = mEntityMap.find(id);
    if(got == mEntityMap.end())
    {
        throw HighlightManagerException("Entity hasn't spawned.");
    }

    ChunkHighlightList highlightList = highlightShape(coordinate);
    ChunkDehighlightList dehighlightList = dehighlightShape(mEntityMap[id]);
    mEntityMap[id] = coordinate;
    return {highlightList, dehighlightList};
}

bool HighlightManager::chunkIsHighlighted(const ChunkCoord& coordinate)
{
    return mRefCounts.count(coordinate) != 0;
}

ChunkHighlightList HighlightManager::highlightShape(const ChunkCoord& loc)
{
    ChunkHighlightList highlightList;

    for(int64_t x = loc.x - mHighlightRadius; x < loc.x + mHighlightRadius + 1; ++x)
    {
        for(int64_t y = loc.y - mHighlightRadius; y < loc.y + mHighlightRadius + 1; ++y)
        {
            for(int64_t z = loc.z - mHighlightRadius; z < loc.z + mHighlightRadius + 1; ++z)
            {
                ChunkCoord subLoc(x, y, z);

                if(glm::distance(glm::vec3(loc), glm::vec3(subLoc)) <= (float)mHighlightRadius)
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

ChunkDehighlightList HighlightManager::dehighlightShape(const ChunkCoord& loc)
{
    ChunkDehighlightList dehighlightList;

    for(int64_t x = loc.x - mHighlightRadius; x < loc.x + mHighlightRadius + 1; ++x)
    {
        for(int64_t y = loc.y - mHighlightRadius; y < loc.y + mHighlightRadius + 1; ++y)
        {
            for(int64_t z = loc.z - mHighlightRadius; z < loc.z + mHighlightRadius + 1; ++z)
            {
                ChunkCoord subLoc(x, y, z);

                if(glm::distance(glm::dvec3(loc), glm::dvec3(subLoc)) <= (double)mHighlightRadius)
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
