#pragma once
#include "../world/chunk.hpp"
#include <memory>
#include <cstdint>
#include "asaddons/scriptarray.hpp"

class ScriptChunk
{
    public:
        ScriptChunk();
        ScriptChunk(Chunk* chunk, asIObjectType* voxelDataArrayType);
        ScriptChunk(const Chunk* chunk, asIObjectType* voxelDataArrayType);
        ScriptChunk(const ScriptChunk& other);
        void addRef();
        void release();
        VoxelType getVoxelType(const ChunkVoxelCoord&);
        CScriptArray* getVoxelData();
        void setVoxelType(const ChunkVoxelCoord&, VoxelType);
        void setVoxelData(const CScriptArray& types);
    private:
        const Chunk* mChunkRO;
        Chunk* mChunk;
        int32_t mRefCount;
        bool mReadOnly;
        asIObjectType* mVoxelDataArrayType;
};

ScriptChunk* constructScriptChunk();
void copyConstructScriptChunk(const ScriptChunk& other, void* pointer);
