#pragma once
#include "chunkvbo.h"
#include "chunk.h"

class VBOCreator
{
    public:
    ChunkVBO generateChunkVBO(const Chunk& chunk) const;
};
