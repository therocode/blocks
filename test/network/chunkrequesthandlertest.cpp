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
            handler.addRequest(1, ChunkCoord(2, 1, 3));
            handler.addRequest(1, ChunkCoord(1, 3, 4));
            
            THEN("The requests are added")
            {
                REQUIRE(handler.getRequestAmount(0) == 1);
                REQUIRE(handler.getRequestAmount(1) == 2);
                REQUIRE(handler.getRequestAmount(2) == 0);
                REQUIRE(handler.getTotalRequestAmount() == 3);
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
            handler.addRequest(0, ChunkCoord(1, 2, 3));
            handler.addRequest(0, ChunkCoord(0, 2, 3));
            handler.addRequest(0, ChunkCoord(1, 2, 3));
            handler.addRequest(0, ChunkCoord(1, 2, 3));
            handler.addRequest(1, ChunkCoord(2, 1, 3));
            handler.addRequest(1, ChunkCoord(2, 1, 3));
            handler.addRequest(1, ChunkCoord(2, 1, 3));
            handler.addRequest(1, ChunkCoord(2, 1, 3));
            handler.addRequest(1, ChunkCoord(2, 1, 3));
            
            THEN("The equal requests are not accounted for")
            {
                REQUIRE(handler.getRequestAmount(0) == 2);
                REQUIRE(handler.getRequestAmount(1) == 1);
                REQUIRE(handler.getTotalRequestAmount() == 3);
            }   
        }   
    }   
}
