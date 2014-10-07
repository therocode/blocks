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
#include "worldconstants.hpp"

#define PR(x) std::cerr << #x << " = " << (x) << std::endl;

#define NO_CHUNK -1

const std::string regionDir = "worlddata";
const std::string dataExt = ".dat";
const std::string indexExt = ".idx";

const std::string pathSep =
#ifdef _WIN32
    "\\";
#else
    "/";
#endif

struct Mod
{
    ChunkVoxelCoord coord;
    VoxelType type;

    Mod() {}
    Mod(ChunkVoxelCoord coord, VoxelType type)
        : coord(coord), type(type) {}
};

//regions are defined locally to only the modmanager
using RegionCoord = glm::i64vec3;
using RegionChunkCoord = glm::u8vec3;
const uint32_t regionChunkWidth = 32;

using ChunkToRegion = CoordinateCoarseConvert<ChunkCoord, RegionCoord, regionChunkWidth>;
using ChunkToRegionChunk = CoordinateWrapConvert<ChunkCoord, RegionChunkCoord, 0, regionChunkWidth - 1>;

using ChunkIndex = uint32_t;
using ChunkModMap = std::unordered_map<ChunkVoxelCoord, VoxelType>;
using RegionModMap = std::unordered_map<RegionChunkCoord, ChunkModMap>;
using WorldModMap = std::unordered_map<RegionCoord, RegionModMap>;
using RegionTimestampMap = std::unordered_map<RegionChunkCoord, uint64_t>;
using WorldTimestampMap = std::unordered_map<RegionCoord, RegionTimestampMap>;

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
        ModManager(const std::string& worldName);
        void loadMods(Chunk& chunk);
        void saveMods();
        void saveMods(RegionCoord regionLoc);
        void setMod(ChunkCoord loc, ChunkVoxelCoord voxLoc, VoxelType type);
        VoxelType getMod(ChunkCoord loc, ChunkVoxelCoord voxLoc);
        void deleteRegionFile(const RegionCoord& regionLoc);
        void recordTimestamp(ChunkCoord loc, uint64_t timestamp);
        void setWorldName(const std::string& worldName);

    private:
        ChunkIndex getChunkIndex(RegionCoord regionLoc, RegionChunkCoord chunkLoc);
        void initIndexFile(RegionCoord regionLoc);
        void _setMod(const RegionCoord& regionLoc, const RegionChunkCoord& chunkLoc, const ChunkVoxelCoord& voxLoc, VoxelType type);
        std::string getFilename(RegionCoord regionLoc);

        RegionCoord mRegionLoc;
        std::string mIndexPath;
        std::string mDataPath;
        WorldModMap mMods;
        WorldTimestampMap mTimestamps;
        std::string mWorldName;
};
