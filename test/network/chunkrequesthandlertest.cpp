#include "../catch.hpp"
#include "../testhelpers.hpp"
#include "../src/world/worlddefines.hpp"

#include "../src/networking/chunkrequesthandler.hpp"

SCENARIO("Chunk request handler can accept requests","[networking]")
{
    GIVEN("A chunk request handler")
    {   
        ChunkRequestHandler handler;

        WHEN("Requests are added to the handler")
        {   
            handler.addRequest(0, ChunkCoord(1, 2, 3));
            handler.addRequest(1, ChunkCoord(1, 2, 3));
            handler.addRequest(2, ChunkCoord(1, 3, 4));
            
            THEN("The requests are added")
            {
                REQUIRE(handler.getRequesterAmount(ChunkCoord(1, 2, 3)) == 2);
                REQUIRE(handler.getRequesterAmount(ChunkCoord(1, 3, 4)) == 1);
                REQUIRE(handler.getRequesterAmount(ChunkCoord(1, 2, 7)) == 0);
                REQUIRE(handler.getTotalRequesterAmount() == 3);
            }   
        }   
    }   
}

SCENARIO("Chunk request that are equal to previous requests are not accounted for","[networking]")
{
    GIVEN("A chunk request handler")
    {   
        ChunkRequestHandler handler;

        WHEN("Requests are added to the handler that are the same as other requests")
        {   
            handler.addRequest(1, ChunkCoord(1, 2, 3));
            handler.addRequest(1, ChunkCoord(1, 2, 3));
            handler.addRequest(2, ChunkCoord(1, 2, 3));
            handler.addRequest(2, ChunkCoord(1, 2, 3));
            handler.addRequest(1, ChunkCoord(2, 1, 3));
            handler.addRequest(1, ChunkCoord(2, 1, 3));
            handler.addRequest(1, ChunkCoord(2, 1, 3));
            handler.addRequest(1, ChunkCoord(2, 1, 3));
            handler.addRequest(1, ChunkCoord(2, 1, 3));
            
            THEN("The equal requests are not accounted for")
            {
                REQUIRE(handler.getRequesterAmount(ChunkCoord(1, 2, 3)) == 2);
                REQUIRE(handler.getRequesterAmount(ChunkCoord(2, 1, 3)) == 1);
                REQUIRE(handler.getTotalRequesterAmount() == 3);
            }   
        }   
    }   
}

SCENARIO("Chunk request can be gotten and terminated","[networking]")
{
    GIVEN("A chunk request handler with requests added")
    {   
        ChunkRequestHandler handler;

        handler.addRequest(1, ChunkCoord(1, 2, 3));
        handler.addRequest(2, ChunkCoord(1, 2, 3));
        handler.addRequest(1, ChunkCoord(2, 1, 3));

        WHEN("Requests are gotten and removed properly")
        {   
            auto result1 = handler.getAndRemove(ChunkCoord(0, 0, 0));
            auto result2 = handler.getAndRemove(ChunkCoord(1, 2, 3));
            auto result3 = handler.getAndRemove(ChunkCoord(2, 1, 3));
            auto result4 = handler.getAndRemove(ChunkCoord(1, 2, 3));

            THEN("The equal requests are not accounted for")
            {
                REQUIRE(result1.size() == 0);
                REQUIRE(result2.size() == 2);
                REQUIRE(result3.size() == 1);
                REQUIRE(result4.size() == 0);

                REQUIRE(std::find(result2.begin(), result2.end(), 1) != result2.end());
                REQUIRE(std::find(result2.begin(), result2.end(), 2) != result2.end());
                REQUIRE(std::find(result3.begin(), result3.end(), 1) != result3.end());
            }   
        }   
    }   
}
