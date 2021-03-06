#include "../catch.hpp"

#include "../src/world/highlightmanager.hpp"

#include <vector>

TEST_CASE("", "[spawn][despawn][move]")
{
    fea::EntityId eId1 = 1;
    fea::EntityId eId2 = 2;
    ChunkCoord loc1(0,0,0);
    ChunkCoord loc2(1,0,0);
    ChunkCoord loc3(5,0,0);
    int highlightRadius = 3;

    HighlightManager manager;

    SECTION("spawn entity") 
    {
        std::vector<ChunkCoord> highlightedLocs = manager.addHighlightEntity(eId1, loc1, highlightRadius);

        for(int64_t x = loc1.x - highlightRadius; x < loc1.x + highlightRadius + 1; ++x)
        {
            for(int64_t y = loc1.y - highlightRadius; y < loc1.y + highlightRadius + 1; ++y)
            {
                for(int64_t z = loc1.z - highlightRadius; z < loc1.z + highlightRadius + 1; ++z)
                {
                    ChunkCoord loc(x, y, z); 
                    std::vector<ChunkCoord>::const_iterator got = find(highlightedLocs.begin(), highlightedLocs.end(), loc);
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

        REQUIRE(manager.chunkIsHighlighted(loc1));
        REQUIRE(manager.chunkIsHighlighted(loc1 + ChunkCoord(1, 1, 1)));
        REQUIRE(manager.chunkIsHighlighted(loc1 - ChunkCoord(1, 1, 1)));
    }

    SECTION("spawn and despawn entity") 
    {
        auto highlightedLocs = manager.addHighlightEntity(eId1, loc1, highlightRadius);
        auto dehighlightedLocs = manager.removeHighlightEntity(eId1);

        REQUIRE(highlightedLocs.size() == dehighlightedLocs.size());

        REQUIRE_FALSE(manager.chunkIsHighlighted(loc1));
        REQUIRE_FALSE(manager.chunkIsHighlighted(loc1 + ChunkCoord(1, 1, 1)));
        REQUIRE_FALSE(manager.chunkIsHighlighted(loc1 - ChunkCoord(1, 1, 1)));
    }

    SECTION("spawn and move entity")
    {
        auto highlightedLocs = manager.addHighlightEntity(eId1, loc1, highlightRadius);
        auto highlighted_dehighlightedLocs = manager.moveHighlightEntity(eId1, loc2);

        REQUIRE(29 == highlighted_dehighlightedLocs.second.size());
    }

    SECTION("intersect two highlight entities")
    {
        auto highlightedLocs1 = manager.addHighlightEntity(eId1, loc1, highlightRadius);
        auto highlightedLocs2 = manager.addHighlightEntity(eId2, loc3, highlightRadius);

        REQUIRE(244 == highlightedLocs1.size() + highlightedLocs2.size());
    }

    SECTION("intersect two highlight entities and despawn one")
    {
        manager.addHighlightEntity(eId1, loc1, highlightRadius);
        manager.addHighlightEntity(eId2, loc3, highlightRadius);
        auto dehighlightedList = manager.removeHighlightEntity(eId1);
        
        REQUIRE(121 == dehighlightedList.size());        
    }
}
