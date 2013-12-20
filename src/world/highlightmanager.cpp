#include "highlightmanager.h"

using namespace std;

HighlightManager::HighlightManager(fea::MessageBus bus)
    : mBus(bus) {}

void HighlightManager::addHighlight(ChunkCoord loc)
{
    RefMap::const_iterator got = refCounts.find(loc);
    if(got == refCounts.end())
    {
        refCounts[loc] = 1;
    }
    else
    {
        ++refCounts[loc];
    }

    if(refCounts[loc] == 1) 
    {
        mBus.sendMessage<ChunkHighlightedMessage>(loc);
    } 
}

void HighlightManager::removeHighlight(ChunkCoord loc)
{
    RefMap::const_iterator got = refCounts.find(loc);
    if(got == refCounts.end())
    {
        throw HighlightManagerException(glm::to_string((glm::ivec3)loc) + " has not been highlighted."); //NOTE: not sure why this needs to be casted... not good.
    }
    else
    {
        if(refCounts[loc] == 0)
        {
            throw HighlightManagerException(glm::to_string((glm::ivec3)loc) + " has not been highlighted."); //NOTE: not sure why this needs to be casted... not good.
        }
        
        --refCounts[loc];

        if(refCounts[loc] == 0)
        {
            mBus.sendMessage<ChunkDehighlightedMessage>(loc);
        }
    }
}
