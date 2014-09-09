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

#include <fea/util.hpp>

#include "chunk.h"
#include "worldmessages.h"
#include "worldstd.h"

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
        ModManager(fea::MessageBus& bus);
        void loadMods(Chunk& chunk);
        void saveMods();
        void saveMods(RegionCoord regionLoc);
        void setMod(ChunkCoord loc, ChunkVoxelCoord voxLoc, VoxelType type);
        VoxelType getMod(ChunkCoord loc, ChunkVoxelCoord voxLoc);
        void deleteRegionFile(const RegionCoord& regionLoc);
        void recordTimestamp(ChunkCoord loc, uint64_t timestamp);

    private:
        ChunkIndex getChunkIndex(RegionCoord regionLoc, RegionChunkCoord chunkLoc);
        void initIndexFile(RegionCoord regionLoc);
        void _setMod(const RegionCoord& regionLoc, const RegionChunkCoord& chunkLoc, const ChunkVoxelCoord& voxLoc, VoxelType type);
        std::string getFilename(RegionCoord regionLoc);

        fea::MessageBus& mBus;
        RegionCoord mRegionLoc;
        std::string mIndexPath;
        std::string mDataPath;
        WorldModMap mMods;
        WorldTimestampMap mTimestamps;
};


