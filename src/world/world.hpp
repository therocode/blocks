#pragma once
#include "chunk.hpp"
#include <unordered_map>
#include "ranges.hpp"

class World
{
    public:
        World(const std::string& title, const Ranges& ranges = Ranges());
        void addChunk(const ChunkCoord& coordinate, const Chunk& chunk);
        const Chunk* findChunk(const ChunkCoord& coordinate) const;
        void removeChunk(const ChunkCoord& coordinate);
    private:

        //world information
        std::string mTextIdentifier;
        std::string mTitle;

        Ranges mWorldRange;

        std::unordered_map<ChunkCoord, Chunk> mChunks;
};
