#include "highlightmanager.h"

using namespace std;

HighlightManager::HighlightManager(fea::MessageBus& bus, int highlightRadius)
    : mBus(bus), mHighlightRadius(highlightRadius) 
{
    mBus.addMessageSubscriber<HighlightEntitySpawnedMessage>(*this);
    mBus.addMessageSubscriber<HighlightEntityDespawnedMessage>(*this);
    mBus.addMessageSubscriber<HighlightEntityMovedMessage>(*this);
}

void HighlightManager::handleMessage(const HighlightEntitySpawnedMessage& msg)
{
    fea::EntityId id;
    ChunkCoord loc;
    tie(id, loc) = msg.data; 

    EntityMap::const_iterator got = mEntityMap.find(id);
    if(got != mEntityMap.end())
    {
        throw HighlightManagerException("Entity already spawned.");
    }

    mEntityMap[id] = loc;
    highlightShape(loc); 
}

void HighlightManager::handleMessage(const HighlightEntityDespawnedMessage& msg)
{
    fea::EntityId id;
    tie(id) = msg.data;

    EntityMap::const_iterator got = mEntityMap.find(id);
    if(got == mEntityMap.end())
    {
        throw HighlightManagerException("Entity hasn't spawned.");
    }

    dehighlightShape(mEntityMap[id]);
    mEntityMap.erase(got);
}

void HighlightManager::handleMessage(const HighlightEntityMovedMessage& msg)
{
    fea::EntityId id;
    ChunkCoord loc;
    tie(id, loc) = msg.data;

    EntityMap::const_iterator got = mEntityMap.find(id);
    if(got == mEntityMap.end())
    {
        throw HighlightManagerException("Entity hasn't spawned.");
    }

    highlightShape(loc);
    dehighlightShape(mEntityMap[id]);
    mEntityMap[id] = loc;
}

void HighlightManager::highlightShape(const ChunkCoord& loc)
{
    for(int64_t x = loc.x - mHighlightRadius; x < loc.x + mHighlightRadius + 1; ++x)
    {
        for(int64_t y = loc.y - mHighlightRadius; y < loc.y + mHighlightRadius + 1; ++y)
        {
            for(int64_t z = loc.z - mHighlightRadius; z < loc.z + mHighlightRadius + 1; ++z)
            {
                ChunkCoord subLoc(x, y, z);

                if(glm::distance(glm::dvec3(loc), glm::dvec3(subLoc)) <= (double)mHighlightRadius)
                {
                    highlightChunk(subLoc);
                }
            }
        }
    }
}

void HighlightManager::dehighlightShape(const ChunkCoord& loc)
{
    for(int64_t x = loc.x - mHighlightRadius; x < loc.x + mHighlightRadius + 1; ++x)
    {
        for(int64_t y = loc.y - mHighlightRadius; y < loc.y + mHighlightRadius + 1; ++y)
        {
            for(int64_t z = loc.z - mHighlightRadius; z < loc.z + mHighlightRadius + 1; ++z)
            {
                ChunkCoord subLoc(x, y, z);

                if(glm::distance(glm::dvec3(loc), glm::dvec3(subLoc)) <= (double)mHighlightRadius)
                {
                    dehighlightChunk(subLoc);
                }
            }
        }
    }
}

void HighlightManager::highlightChunk(const ChunkCoord& coord)
{
    RefMap::const_iterator got = mRefCounts.find(coord);
    if(got == mRefCounts.end())
    {
        mRefCounts[coord] = 0;
    }

    ++mRefCounts[coord];

    if(mRefCounts[coord] == 1) 
    {
        mBus.sendMessage<ChunkHighlightedMessage>(ChunkHighlightedMessage(coord));    
    }
}

void HighlightManager::dehighlightChunk(const ChunkCoord& coord)
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
        mBus.sendMessage<ChunkDehighlightedMessage>(ChunkDehighlightedMessage(coord));
    }
}
