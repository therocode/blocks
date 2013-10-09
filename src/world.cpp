#include "world.h"

void World::initialise()
{
    renderer.setup();
    Chunk newChunk;

    chunks.push_back(newChunk);

    renderer.addChunk(newChunk);
}

void World::update()
{
    renderer.render();
}

void World::hehehe()
{
    renderer.hehehe();
}
