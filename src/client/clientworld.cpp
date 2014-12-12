#include "clientworld.hpp"
#include "../rendering/renderingmessages.hpp"
#include "../networking/networkingprotocol.hpp"

ClientWorld::ClientWorld(fea::MessageBus& bus) :
    mBus(bus)
{
    subscribe(mBus, *this);
}

void ClientWorld::handleMessage(const ChunksDataDeliveredMessage& received)
{
    std::cout << "HEJHEJHEJHEJEHJ\n";
    for(const auto& chunkIter : received.chunks)
    {
        const ChunkCoord& coordinate = chunkIter.first;

        auto worldEntry = mWorldEntries.find(received.worldId);

        if(worldEntry != mWorldEntries.end())
        {
            if(worldEntry->second.highlightManager.chunkIsHighlighted(coordinate))
            {
                WorldId id = received.worldId;
                const Chunk& chunk = chunkIter.second;

                auto& voxels = worldEntry->second.voxels;

                voxels[coordinate] = chunk;

                updateChunk(id, coordinate);

                if(voxels.find(ChunkCoord(coordinate.x + 1, coordinate.y, coordinate.z)) != voxels.end())
                {
                    updateChunk(id, ChunkCoord(coordinate.x + 1, coordinate.y, coordinate.z));
                }
                if(voxels.find(ChunkCoord(coordinate.x - 1, coordinate.y, coordinate.z)) != voxels.end())
                {
                    updateChunk(id, ChunkCoord(coordinate.x - 1, coordinate.y, coordinate.z));
                }
                if(voxels.find(ChunkCoord(coordinate.x, coordinate.y + 1, coordinate.z)) != voxels.end())
                {
                    updateChunk(id, ChunkCoord(coordinate.x, coordinate.y + 1, coordinate.z));
                }
                if(voxels.find(ChunkCoord(coordinate.x, coordinate.y - 1, coordinate.z)) != voxels.end())
                {
                    updateChunk(id, ChunkCoord(coordinate.x, coordinate.y - 1, coordinate.z));
                }
                if(voxels.find(ChunkCoord(coordinate.x, coordinate.y, coordinate.z + 1)) != voxels.end())
                {
                    updateChunk(id, ChunkCoord(coordinate.x, coordinate.y, coordinate.z + 1));
                }
                if(voxels.find(ChunkCoord(coordinate.x, coordinate.y, coordinate.z - 1)) != voxels.end())
                {
                    updateChunk(id, ChunkCoord(coordinate.x, coordinate.y, coordinate.z - 1));
                }
            }
        }
    }
}

void ClientWorld::handleMessage(const VoxelSetMessage& received)
{
    ChunkCoord chunkCoord = VoxelToChunk::convert(received.voxel);
    ChunkVoxelCoord chunkVoxelCoord = VoxelToChunkVoxel::convert(received.voxel);
    WorldId id = received.worldId;

    auto worldEntry = mWorldEntries.find(id);

    if(worldEntry != mWorldEntries.end())
    {
        auto& voxels = worldEntry->second.voxels;

        auto chunk = voxels.find(chunkCoord);

        if(chunk != voxels.end())
        {
            chunk->second.setVoxelType(chunkVoxelCoord, received.type);
            updateChunk(id, chunkCoord);
        }

        ChunkCoord leftNeighbour = ChunkCoord(chunkCoord.x - 1, chunkCoord.y, chunkCoord.z);
        if(chunkVoxelCoord.x == 0 && voxels.find(leftNeighbour) != voxels.end())
        {
            updateChunk(id, leftNeighbour);
        }

        ChunkCoord rightNeighbour = ChunkCoord(chunkCoord.x + 1, chunkCoord.y, chunkCoord.z);
        if(chunkVoxelCoord.x == chunkWidth - 1 && voxels.find(rightNeighbour) != voxels.end())
        {
            updateChunk(id, rightNeighbour);
        }

        ChunkCoord topNeighbour = ChunkCoord(chunkCoord.x, chunkCoord.y + 1, chunkCoord.z);
        if(chunkVoxelCoord.y == chunkWidth - 1 && voxels.find(topNeighbour) != voxels.end())
        {
            updateChunk(id, topNeighbour);
        }

        ChunkCoord bottomNeighbour = ChunkCoord(chunkCoord.x, chunkCoord.y - 1, chunkCoord.z);
        if(chunkVoxelCoord.y == 0 && voxels.find(bottomNeighbour) != voxels.end())
        {
            updateChunk(id, bottomNeighbour);
        }

        ChunkCoord frontNeighbour = ChunkCoord(chunkCoord.x, chunkCoord.y, chunkCoord.z + 1);
        if(chunkVoxelCoord.z == chunkWidth - 1 && voxels.find(frontNeighbour) != voxels.end())
        {
            updateChunk(id, frontNeighbour);
        }

        ChunkCoord backNeighbour = ChunkCoord(chunkCoord.x, chunkCoord.y, chunkCoord.z - 1);
        if(chunkVoxelCoord.z == 0 && voxels.find(backNeighbour) != voxels.end())
        {
            updateChunk(id, backNeighbour);
        }
    }
}

void ClientWorld::handleMessage(const ChunkDeletedMessage& received)
{
    auto worldEntry = mWorldEntries.find(received.worldId);

    if(worldEntry != mWorldEntries.end())
    {
        auto& voxels = worldEntry->second.voxels;
        voxels.erase(received.coordinate);
    }
}

void ClientWorld::handleMessage(const HighlightEntityAddRequestedMessage& received)
{
    std::cout << "wahoo\n";
    auto highlighted = mWorldEntries[received.worldId].highlightManager.addHighlightEntity(0, received.coordinate, received.radius);
    //mLastChunk = WorldToChunk::convert(received.position);

    if(highlighted.size() > 0)
        mBus.send(ChunksRequestedMessage{received.worldId, highlighted});
}

void ClientWorld::updateChunk(WorldId id, const ChunkCoord& coordinate)
{
    FEA_ASSERT(mWorldEntries.count(id), "Updating non-existing chunk? weird");

    auto& voxels = mWorldEntries.at(id).voxels;

    Chunk* mainChunk = &voxels.at(coordinate);
    Chunk* topChunk = nullptr;
    Chunk* bottomChunk = nullptr;
    Chunk* frontChunk = nullptr;
    Chunk* backChunk = nullptr;
    Chunk* leftChunk = nullptr;
    Chunk* rightChunk = nullptr;

    auto top    = voxels.find(ChunkCoord(coordinate.x, coordinate.y + 1, coordinate.z));
    auto bottom = voxels.find(ChunkCoord(coordinate.x, coordinate.y - 1, coordinate.z));
    auto front  = voxels.find(ChunkCoord(coordinate.x, coordinate.y, coordinate.z + 1));
    auto back   = voxels.find(ChunkCoord(coordinate.x, coordinate.y, coordinate.z - 1));
    auto left   = voxels.find(ChunkCoord(coordinate.x - 1, coordinate.y, coordinate.z));
    auto right  = voxels.find(ChunkCoord(coordinate.x + 1, coordinate.y, coordinate.z));

    if(top != voxels.end())
        topChunk = &top->second;
    if(bottom != voxels.end())
        bottomChunk = &bottom->second;
    if(front != voxels.end())
        frontChunk = &front->second;
    if(back != voxels.end())
        backChunk = &back->second;
    if(left != voxels.end())
        leftChunk = &left->second;
    if(right != voxels.end())
        rightChunk = &right->second;

    mBus.send(UpdateChunkVboMessage{coordinate, mainChunk, topChunk, bottomChunk, frontChunk, backChunk, leftChunk, rightChunk});
}

const ChunkMap& ClientWorld::getVoxels(WorldId worldId) const
{
    FEA_ASSERT(mWorldEntries.count(worldId) > 0, "Cannot get voxels of unexisting world");
    return mWorldEntries.at(worldId).voxels;
}
