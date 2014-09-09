#pragma once

#include <iostream>
#include <unordered_map>

#include <fea/util.hpp>

#include "chunk.h"
#include "worldmessages.h"
#include "worldstd.h"

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

class HighlightManager : 
    public fea::MessageReceiver<HighlightEntitySpawnedMessage>,
    public fea::MessageReceiver<HighlightEntityDespawnedMessage>,
    public fea::MessageReceiver<HighlightEntityMovedMessage>
{
    public:
        HighlightManager(fea::MessageBus& bus, int highlightRadius);

    private:
        virtual void handleMessage(const HighlightEntitySpawnedMessage& msg);
        virtual void handleMessage(const HighlightEntityDespawnedMessage& msg);
        virtual void handleMessage(const HighlightEntityMovedMessage& msg);
        void highlightShape(const ChunkCoord& coord);
        void dehighlightShape(const ChunkCoord& coord);
        void highlightChunk(const ChunkCoord& coord);
        void dehighlightChunk(const ChunkCoord& coord);

        fea::MessageBus& mBus;
        RefMap mRefCounts;
        EntityMap mEntityMap;
        int mHighlightRadius;
};
