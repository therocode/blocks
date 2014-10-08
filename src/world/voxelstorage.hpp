#pragma once
#include <unordered_map>
#include "worlddefines.hpp"
#include "chunk.hpp"

using VoxelStorage = std::unordered_map<ChunkCoord, Chunk>;
