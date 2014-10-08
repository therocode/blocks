#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include "../src/world/voxelstorage.hpp"

using ChunkVoxelCoord  = glm::u8vec3;

SCENARIO("Chunks can be added to worlds and gotten back","[world]")
{
    GIVEN("A newly created world and a couple of chunks")
    {   
        World world("test_world");
        Chunk chunkA;
        Chunk chunkB;

        WHEN("A chunk is gotten from an empty world")
        {
            const Chunk* gotten = world.findChunk({0, 0, 0});

            THEN("The chunk is nullptr")
            {
                REQUIRE(gotten == nullptr);
            }
        }

        WHEN("The chunks are added to the world and gotten")
        {   
            world.addChunk({0, 0, 0}, chunkA);
            world.addChunk({-4, 1, 100}, chunkB);

            const Chunk* gottenA = world.findChunk({0, 0, 0});
            const Chunk* gottenB = world.findChunk({-4, 1, 100});

            THEN("The accessed chunks are the same as the originals")
            {   
                REQUIRE(gottenA != nullptr);
                REQUIRE(gottenB != nullptr);
                REQUIRE(chunkA == *gottenA);
                REQUIRE(chunkB == *gottenB);
            }
        }

        WHEN("A chunk is added and subsequently removed")
        {
            Chunk chunk;
            world.addChunk({1, 1, 1}, chunk);

            world.removeChunk({1, 1, 1});

            THEN("The chunk is not existing anymore")
            {
                REQUIRE(world.findChunk({1, 1, 1}) == nullptr);
            }
        }
    }
}
