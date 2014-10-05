#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include <iostream>
#include <tuple>

#include <fea/util.hpp>

#include "../src/world/highlightmanager.hpp"

#define PR(x) std::cerr << #x << " = " << (x) << std::endl;

using namespace std;

TEST_CASE("", "[spawn][despawn][move]")
{
    fea::EntityId eId1 = 1;
    fea::EntityId eId2 = 2;
    ChunkCoord loc1(0,0,0);
    ChunkCoord loc2(1,0,0);
    ChunkCoord loc3(5,0,0);
    int highlightRadius = 3;

    fea::MessageBus bus;
    HighlightManager manager(highlightRadius);

    SECTION("spawn entity") 
    {
        vector<ChunkCoord> highlightedLocs = manager.addHighlightEntity(eId1, loc1);

        for(int64_t x = loc1.x - highlightRadius; x < loc1.x + highlightRadius + 1; ++x)
        {
            for(int64_t y = loc1.y - highlightRadius; y < loc1.y + highlightRadius + 1; ++y)
            {
                for(int64_t z = loc1.z - highlightRadius; z < loc1.z + highlightRadius + 1; ++z)
                {
                    ChunkCoord loc(x, y, z); 
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
        auto highlightedLocs = manager.addHighlightEntity(eId1, loc1);
        auto dehighlightedLocs = manager.removeHighlightEntity(eId1);

        REQUIRE(highlightedLocs.size() == dehighlightedLocs.size());
    }

    SECTION("spawn and move entity")
    {
        auto highlightedLocs = manager.addHighlightEntity(eId1, loc1);
        auto highlighted_dehighlightedLocs = manager.moveHighlightEntity(eId1, loc2);

        REQUIRE(29 == highlighted_dehighlightedLocs.second.size());
    }

    SECTION("intersect two highlight entities")
    {
        auto highlightedLocs1 = manager.addHighlightEntity(eId1, loc1);
        auto highlightedLocs2 = manager.addHighlightEntity(eId2, loc3);

        REQUIRE(244 == highlightedLocs1.size() + highlightedLocs2.size());
    }

    SECTION("intersect two highlight entities and despawn one")
    {
        manager.addHighlightEntity(eId1, loc1);
        manager.addHighlightEntity(eId2, loc3);
        auto dehighlightedList = manager.removeHighlightEntity(eId1);
        
        REQUIRE(121 == dehighlightedList.size());        
    }
}
