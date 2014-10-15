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
