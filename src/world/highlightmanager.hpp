#pragma once

#include <iostream>
#include <unordered_map>
#include <fea/entitysystem.hpp>

#include "chunk.hpp"
#include "worldstd.hpp"

#define PR(x) std::cerr << #x << " = " << (x) << std::endl;

using RefMap = std::unordered_map<ChunkCoord, uint16_t>;
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
        HighlightManager(int highlightRadius);
        ChunkHighlightList addHighlightEntity(fea::EntityId id, const ChunkCoord& coordinate);
        ChunkDehighlightList removeHighlightEntity(fea::EntityId id);
        std::pair<ChunkHighlightList, ChunkDehighlightList> moveHighlightEntity(fea::EntityId id, const ChunkCoord& coordinate);

    private:
        ChunkHighlightList highlightShape(const ChunkCoord& coord);
        ChunkDehighlightList dehighlightShape(const ChunkCoord& coord);
        bool highlightChunk(const ChunkCoord& coord);
        bool dehighlightChunk(const ChunkCoord& coord);

        RefMap mRefCounts;
        EntityMap mEntityMap;
        int mHighlightRadius;
};
