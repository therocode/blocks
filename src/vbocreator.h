#pragma once
#include "chunkvbo.h"
#include "chunk.h"

class VBOCreator
{
    ChunkVBO generateChunkVBO(const Chunk& chunk) const;
};
