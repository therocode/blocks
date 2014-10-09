#pragma once
#include <unordered_map>
#include "worlddefines.hpp"
#include "chunk.hpp"

using ChunkMap = std::unordered_map<ChunkCoord, Chunk>;
