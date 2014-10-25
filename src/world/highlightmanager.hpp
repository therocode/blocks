#pragma once

#include <iostream>
#include <unordered_map>
#include <fea/entitysystem.hpp>

#include "chunk.hpp"
#include "worlddefines.hpp"

#define PR(x) std::cerr << #x << " = " << (x) << std::endl;

using RefMap = std::unordered_map<ChunkCoord, uint32_t>;
using EntityMap = std::unordered_map<fea::EntityId, ChunkCoord>;

struct HighlightManagerException : public std::exception
{
    std::string s;
    HighlightManagerException(std::string ss) : s(ss) {}
    ~HighlightManagerException() throw () {} // Updated
    const char* what() const throw() { return s.c_str(); }
};

using ChunkHighlightList = std::vector<ChunkCoord>;
using ChunkDehighlightList = std::vector<ChunkCoord>;

class HighlightManager
{
    public:
        ChunkHighlightList addHighlightEntity(fea::EntityId id, const ChunkCoord& coordinate, uint32_t radius);
        ChunkDehighlightList removeHighlightEntity(fea::EntityId id);
        std::pair<ChunkHighlightList, ChunkDehighlightList> moveHighlightEntity(fea::EntityId id, const ChunkCoord& coordinate);
        bool chunkIsHighlighted(const ChunkCoord& coordinate);
    private:
        ChunkHighlightList highlightShape(const ChunkCoord& coord, uint32_t radius);
        ChunkDehighlightList dehighlightShape(const ChunkCoord& coord, uint32_t radius);
        bool highlightChunk(const ChunkCoord& coord);
        bool dehighlightChunk(const ChunkCoord& coord);

        RefMap mRefCounts;
        EntityMap mEntityMap;
        std::unordered_map<fea::EntityId, uint32_t> mRadii;
};
