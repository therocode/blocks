#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include <iostream>
#include <tuple>

#include <fea/util.hpp>

#include "world/highlightmanager.hpp"
#include "world/worldmessages.hpp"

#define PR(x) std::cerr << #x << " = " << (x) << std::endl;

using namespace std;

class TestReceiver : 
    public fea::MessageReceiver<ChunkHighlightedMessage>,
    public fea::MessageReceiver<ChunkDehighlightedMessage>
{
    public:
        vector<ChunkCoord> getHighlightedLocs()
        {
            return mHighlightedLocs;
        }

        vector<ChunkCoord> getDehighlightedLocs()
        {
            return mDehighlightedLocs;
        }
        
        void clear()
        {
            mHighlightedLocs.clear();
            mDehighlightedLocs.clear();
        }
    
    private:
        void handleMessage(const ChunkHighlightedMessage& message)
        {
            ChunkCoord loc = message.coordinate;
            mHighlightedLocs.push_back(loc);
        }

        void handleMessage(const ChunkDehighlightedMessage& message)
        {
            ChunkCoord loc = message.coordinate;
            mDehighlightedLocs.push_back(loc);
        } 

        vector<ChunkCoord> mHighlightedLocs;
        vector<ChunkCoord> mDehighlightedLocs; 
};

TEST_CASE("", "[spawn][despawn][move]")
{
    fea::EntityId eId1 = 1;
    fea::EntityId eId2 = 2;
    ChunkCoord loc1(0,0,0);
    ChunkCoord loc2(1,0,0);
    ChunkCoord loc3(5,0,0);
    int highlightRadius = 3;

    fea::MessageBus bus;
    TestReceiver receiver;
    bus.addSubscriber<ChunkHighlightedMessage>(receiver);
    bus.addSubscriber<ChunkDehighlightedMessage>(receiver);
    HighlightManager manager(bus, highlightRadius);

    SECTION("spawn entity") 
    {
        bus.send<HighlightEntitySpawnedMessage>(HighlightEntitySpawnedMessage{eId1, loc1});

        for(int64_t x = loc1.x - highlightRadius; x < loc1.x + highlightRadius + 1; ++x)
        {
            for(int64_t y = loc1.y - highlightRadius; y < loc1.y + highlightRadius + 1; ++y)
            {
                for(int64_t z = loc1.z - highlightRadius; z < loc1.z + highlightRadius + 1; ++z)
                {
                    ChunkCoord loc(x, y, z); 
                    vector<ChunkCoord> highlightedLocs = receiver.getHighlightedLocs();
                    vector<ChunkCoord>::const_iterator got = find(highlightedLocs.begin(), highlightedLocs.end(), loc);
                    if(got == highlightedLocs.end())
                    {
                        REQUIRE(glm::distance(glm::dvec3(loc), glm::dvec3(loc1)) > highlightRadius);
                    } 
                    else
                    {
                        REQUIRE(glm::distance(glm::dvec3(loc), glm::dvec3(loc1)) <= highlightRadius);
                    }
                }
            }
        }
    }

    SECTION("spawn and despawn entity") 
    {
        bus.send<HighlightEntitySpawnedMessage>(HighlightEntitySpawnedMessage{eId1, loc1});
        bus.send<HighlightEntityDespawnedMessage>(HighlightEntityDespawnedMessage{eId1});

        REQUIRE(receiver.getHighlightedLocs().size() == receiver.getDehighlightedLocs().size());
    }

    SECTION("spawn and move entity")
    {
        bus.send<HighlightEntitySpawnedMessage>(HighlightEntitySpawnedMessage{eId1, loc1});
        bus.send<HighlightEntityMovedMessage>(HighlightEntityMovedMessage{eId1, loc2});

        REQUIRE(29 == receiver.getDehighlightedLocs().size());
    }

    SECTION("intersect two highlight entities")
    {
        bus.send<HighlightEntitySpawnedMessage>(HighlightEntitySpawnedMessage{eId1, loc1});
        bus.send<HighlightEntitySpawnedMessage>(HighlightEntitySpawnedMessage{eId2, loc3});

        REQUIRE(244 == receiver.getHighlightedLocs().size());
    }

    SECTION("intersect two highlight entities and despawn one")
    {
        bus.send<HighlightEntitySpawnedMessage>(HighlightEntitySpawnedMessage{eId1, loc1});
        bus.send<HighlightEntitySpawnedMessage>(HighlightEntitySpawnedMessage{eId2, loc3});
        bus.send<HighlightEntityDespawnedMessage>(HighlightEntityDespawnedMessage{eId1});
        
        REQUIRE(121 == receiver.getDehighlightedLocs().size());        
    }
}
