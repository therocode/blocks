#include "clientworld.hpp"

ClientWorld::ClientWorld(fea::MessageBus& bus) :
    mBus(bus)
{
    subscribe(mBus, *this);
}

void ClientWorld::handleMessage(const ClientChunksDeliveredMessage& received)
{
}

void ClientWorld::updateVoxelLookAt()
{
	glm::vec3 direction = glm::vec3(glm::cos(mPitch) * glm::sin(mYaw), glm::sin(mPitch), glm::cos(mPitch) * glm::cos(mYaw));

	VoxelCoord block;
	uint32_t face = 0;
	bool f = RayCaster::getVoxelAtRay(mLocalChunks, mPosition + glm::vec3(0, 0.6f, 0), direction, 200.f, face, block);

    mBus.send(FacingBlockMessage{block});
}

void ClientWorld::handleMessage(const ClientChunksDeliveredMessage& received)
{
    for(uint32_t i = 0; i < received.coordinates.size(); i++)
    {
        const ChunkCoord& coordinate = received.coordinates[i];
        if(mHighlightedChunks.chunkIsHighlighted(coordinate))
        {
            Chunk chunk(received.rleIndices[i], received.rleSegments[i]);

            mLocalChunks[coordinate] = chunk;

            updateChunk(coordinate);

            if(mLocalChunks.find(ChunkCoord(coordinate.x + 1, coordinate.y, coordinate.z)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoord(coordinate.x + 1, coordinate.y, coordinate.z));
            }
            if(mLocalChunks.find(ChunkCoord(coordinate.x - 1, coordinate.y, coordinate.z)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoord(coordinate.x - 1, coordinate.y, coordinate.z));
            }
            if(mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y + 1, coordinate.z)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoord(coordinate.x, coordinate.y + 1, coordinate.z));
            }
            if(mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y - 1, coordinate.z)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoord(coordinate.x, coordinate.y - 1, coordinate.z));
            }
            if(mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y, coordinate.z + 1)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoord(coordinate.x, coordinate.y, coordinate.z + 1));
            }
            if(mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y, coordinate.z - 1)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoord(coordinate.x, coordinate.y, coordinate.z - 1));
            }
        }
    }
}

void ClientWorld::handleMessage(const VoxelUpdatedMessage& received)
{
    ChunkCoord chunkCoord = VoxelToChunk::convert(received.voxel);
    ChunkVoxelCoord chunkVoxelCoord = VoxelToChunkVoxel::convert(received.voxel);

    auto chunk = mLocalChunks.find(chunkCoord);

    if(chunk != mLocalChunks.end())
    {
        chunk->second.setVoxelType(chunkVoxelCoord, received.type);
        updateChunk(chunkCoord);
    }

    ChunkCoord leftNeighbour = ChunkCoord(chunkCoord.x - 1, chunkCoord.y, chunkCoord.z);
    if(chunkVoxelCoord.x == 0 && mLocalChunks.find(leftNeighbour) != mLocalChunks.end())
    {
        updateChunk(leftNeighbour);
    }

    ChunkCoord rightNeighbour = ChunkCoord(chunkCoord.x + 1, chunkCoord.y, chunkCoord.z);
    if(chunkVoxelCoord.x == chunkWidth - 1 && mLocalChunks.find(rightNeighbour) != mLocalChunks.end())
    {
        updateChunk(rightNeighbour);
    }

    ChunkCoord topNeighbour = ChunkCoord(chunkCoord.x, chunkCoord.y + 1, chunkCoord.z);
    if(chunkVoxelCoord.y == chunkWidth - 1 && mLocalChunks.find(topNeighbour) != mLocalChunks.end())
    {
        updateChunk(topNeighbour);
    }

    ChunkCoord bottomNeighbour = ChunkCoord(chunkCoord.x, chunkCoord.y - 1, chunkCoord.z);
    if(chunkVoxelCoord.y == 0 && mLocalChunks.find(bottomNeighbour) != mLocalChunks.end())
    {
        updateChunk(bottomNeighbour);
    }

    ChunkCoord frontNeighbour = ChunkCoord(chunkCoord.x, chunkCoord.y, chunkCoord.z + 1);
    if(chunkVoxelCoord.z == chunkWidth - 1 && mLocalChunks.find(frontNeighbour) != mLocalChunks.end())
    {
        updateChunk(frontNeighbour);
    }

    ChunkCoord backNeighbour = ChunkCoord(chunkCoord.x, chunkCoord.y, chunkCoord.z - 1);
    if(chunkVoxelCoord.z == 0 && mLocalChunks.find(backNeighbour) != mLocalChunks.end())
    {
        updateChunk(backNeighbour);
    }
}

void ClientWorld::handleMessage(const ClientChunkDeletedMessage& received)
{
    mLocalChunks.erase(received.coordinate);
}

void ClientWorld::updateChunk(const ChunkCoord& coordinate)
{
    Chunk* mainChunk = &mLocalChunks.at(coordinate);
    Chunk* topChunk = nullptr;
    Chunk* bottomChunk = nullptr;
    Chunk* frontChunk = nullptr;
    Chunk* backChunk = nullptr;
    Chunk* leftChunk = nullptr;
    Chunk* rightChunk = nullptr;

    auto top    = mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y + 1, coordinate.z));
    auto bottom = mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y - 1, coordinate.z));
    auto front  = mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y, coordinate.z + 1));
    auto back   = mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y, coordinate.z - 1));
    auto left   = mLocalChunks.find(ChunkCoord(coordinate.x - 1, coordinate.y, coordinate.z));
    auto right  = mLocalChunks.find(ChunkCoord(coordinate.x + 1, coordinate.y, coordinate.z));

    if(top != mLocalChunks.end())
        topChunk = &top->second;
    if(bottom != mLocalChunks.end())
        bottomChunk = &bottom->second;
    if(front != mLocalChunks.end())
        frontChunk = &front->second;
    if(back != mLocalChunks.end())
        backChunk = &back->second;
    if(left != mLocalChunks.end())
        leftChunk = &left->second;
    if(right != mLocalChunks.end())
        rightChunk = &right->second;

    mBus.send(UpdateChunkVboMessage{coordinate, mainChunk, topChunk, bottomChunk, frontChunk, backChunk, leftChunk, rightChunk});
}
