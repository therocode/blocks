#include "scriptchunk.hpp"
#include <fea/assert.hpp>
#include <fea/freetype-gl/vec234.h>
#include "../world/worlddefines.hpp"

ScriptChunk* constructScriptChunk() 
{
	return new ScriptChunk();
}

void copyConstructScriptChunk(const ScriptChunk& other, void* pointer)
{
	new(pointer) ScriptChunk(other);
}

ScriptChunk::ScriptChunk() 
	: mChunk(nullptr), mChunkRO(nullptr), mRefCount(1)
{
}

ScriptChunk::ScriptChunk(Chunk* chunk, asIObjectType* voxelDataArrayType) 
	: mChunk(chunk), mChunkRO(nullptr), mReadOnly(false), mRefCount(1), mVoxelDataArrayType(voxelDataArrayType)
{
}

ScriptChunk::ScriptChunk(const Chunk* chunk, asIObjectType* voxelDataArrayType)
	: mChunk(nullptr), mChunkRO(chunk), mReadOnly(true), mRefCount(1), mVoxelDataArrayType(voxelDataArrayType)
{
}
	
ScriptChunk::ScriptChunk(const ScriptChunk& other)
	: mChunk(other.mChunk), mChunkRO(other.mChunkRO), mReadOnly(other.mReadOnly), mRefCount(1)
{	
}

void ScriptChunk::addRef()
{
	mRefCount++;
}

void ScriptChunk::release()
{
	mRefCount--;
	if(mRefCount <= 0)
	{
		delete this;
	}
}

uint16_t ScriptChunk::getVoxelType(const ChunkVoxelCoord& coord) 
{
	if(mReadOnly)
		return mChunkRO->getVoxelType(coord);
	else
		return mChunk->getVoxelType(coord);
}

CScriptArray* ScriptChunk::getVoxelData()
{
	CScriptArray* outBuffer = CScriptArray::Create(mVoxelDataArrayType, chunkWidthPow3);
	
	VoxelTypeArray inBuffer = (mReadOnly ? mChunkRO : mChunk)->getFlatVoxelTypeData();
	
	for(int i = 0; i < inBuffer.size(); i++)
		outBuffer->SetValue(i, &inBuffer[i]);
	
	return outBuffer;
}

void ScriptChunk::setVoxelType(const ChunkVoxelCoord& coord, VoxelType type)
{
	FEA_ASSERT(!mReadOnly, "setVoxelType() called on read-only ScriptChunk instance");
	
	mChunk->setVoxelType(coord, type);
}

void ScriptChunk::setVoxelData(const CScriptArray& types)
{
	FEA_ASSERT(!mReadOnly, "setVoxelData() called on read-only ScriptChunk instance");
	
	VoxelTypeArray arr;
	
	for(int i = 0; i < types.GetSize(); i++)
		arr[i] = *((VoxelType*)types.At(i));
	
	mChunk->setVoxelData(arr);
}