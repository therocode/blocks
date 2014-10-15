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
            handler.addRequester(ChunkCoord(1, 2, 3), 0);
            handler.addRequester(ChunkCoord(1, 2, 3), 1);
            handler.addRequester(ChunkCoord(1, 3, 4), 2);
            
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
            handler.addRequester(ChunkCoord(1, 2, 3), 1);
            handler.addRequester(ChunkCoord(1, 2, 3), 1);
            handler.addRequester(ChunkCoord(1, 2, 3), 2);
            handler.addRequester(ChunkCoord(1, 2, 3), 2);
            handler.addRequester(ChunkCoord(2, 1, 3), 1);
            handler.addRequester(ChunkCoord(2, 1, 3), 1);
            handler.addRequester(ChunkCoord(2, 1, 3), 1);
            handler.addRequester(ChunkCoord(2, 1, 3), 1);
            handler.addRequester(ChunkCoord(2, 1, 3), 1);
            
            THEN("The equal requests are not accounted for")
            {
                REQUIRE(handler.getRequesterAmount(ChunkCoord(1, 2, 3)) == 2);
                REQUIRE(handler.getRequesterAmount(ChunkCoord(2, 1, 3)) == 1);
                REQUIRE(handler.getTotalRequesterAmount() == 3);
            }   
        }   
    }   
}
