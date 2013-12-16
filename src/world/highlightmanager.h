#pragma once

#include <unordered_map>

#include <featherkit/messaging.h>
#include <glm/gtx/string_cast.hpp>

#include "chunk.h"
#include "worldmessages.h"
#include "worldstd.h"

using RefMap = std::unordered_map<ChunkCoordinate, uint16_t>;

struct HighlightManagerException : public std::exception
{
    std::string s;
    HighlightManagerException(std::string ss) : s(ss) {}
    ~HighlightManagerException() throw () {} // Updated
    const char* what() const throw() { return s.c_str(); }
};

class HighlightManager
{
    public:
        HighlightManager(fea::MessageBus bus);
        void addHighlight(ChunkCoordinate loc);
        void removeHighlight(ChunkCoordinate loc);

    private:
        fea::MessageBus mBus;
        RefMap refCounts;

};
