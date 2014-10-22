#pragma once

#include <cstdint>
#include <cstdio>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "chunk.hpp"
#include "worlddefines.hpp"

#define PR(x) std::cerr << #x << " = " << (x) << std::endl;

#define NO_CHUNK -1

const std::string modManagerExt = ".dat";
const std::string modManagerIndexExt = ".idx";

struct Mod
{
    ChunkVoxelCoord coord;
    VoxelType type;

    Mod() {}
    Mod(ChunkVoxelCoord coord, VoxelType type)
        : coord(coord), type(type) {}
};

//regions are defined locally to only the modmanager
using ModRegionCoord = glm::i64vec3;
using ModRegionChunkCoord = glm::u8vec3;
const uint32_t modRegionWidthInChunks = 32;
const uint32_t modRegionWidth = modRegionWidthInChunks * chunkWidth;

using VoxelToModRegion = CoordinateCoarseConvert<ChunkCoord, ModRegionCoord, modRegionWidth>;
using ChunkToModRegion = CoordinateCoarseConvert<ChunkCoord, ModRegionCoord, modRegionWidthInChunks>;
using ChunkToModRegionChunk = CoordinateWrapConvert<ChunkCoord, ModRegionChunkCoord, 0, modRegionWidthInChunks - 1>;

using ChunkIndex = uint32_t;
using ChunkModMap = std::unordered_map<ChunkVoxelCoord, VoxelType>;
using ModRegionModMap = std::unordered_map<ModRegionChunkCoord, ChunkModMap>;
using WorldModMap = std::unordered_map<ModRegionCoord, ModRegionModMap>;
using ModRegionTimestampMap = std::unordered_map<ModRegionChunkCoord, uint64_t>;
using WorldTimestampMap = std::unordered_map<ModRegionCoord, ModRegionTimestampMap>;

struct ModManagerException : public std::exception
{
    std::string s;
    ModManagerException(std::string ss) : s(ss) {}
    ~ModManagerException() throw () {} // Updated
    const char* what() const throw() { return s.c_str(); }
};

class ModManager 
{
    public:
        ModManager(const std::string& worldPath);
        void loadMods(const ChunkCoord& location, Chunk& chunk);
        void saveMods();
        void saveMods(ModRegionCoord regionLoc);
        void setMod(const VoxelCoord& voxLoc, VoxelType type);
        VoxelType getMod(const VoxelCoord& voxLoc);
        void deleteModRegionFile(const ModRegionCoord& regionLoc);
        void recordTimestamp(ChunkCoord loc, uint64_t timestamp);
        void setWorldName(const std::string& worldName);

    private:
        ChunkIndex getChunkIndex(ModRegionCoord regionLoc, ModRegionChunkCoord chunkLoc);
        void initIndexFile(ModRegionCoord regionLoc);
        void _setMod(const ModRegionCoord& regionLoc, const ModRegionChunkCoord& chunkLoc, const ChunkVoxelCoord& voxLoc, VoxelType type);
        std::string getFilename(ModRegionCoord regionLoc);

        ModRegionCoord mModRegionLoc;
        std::string mIndexPath;
        std::string mDataPath;
        WorldModMap mMods;
        WorldTimestampMap mTimestamps;
        std::string mWorldPath;
};
