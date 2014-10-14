#include "../utilities/glm.hpp"
#include "vbocreator.hpp"
#include "../world/chunk.hpp"
#include <vector>
#include <map>
#include "../utilities/timer.hpp"

struct ChunkQueueThing{
    ChunkCoord mainChunkCoord;
    Chunk* mainChunk;
    Chunk* topChunk;
    Chunk* bottomChunk;
    Chunk* frontChunk;
    Chunk* backChunk;
    Chunk* leftChunk;
    Chunk* rightChunk;
};
class ChunkGeneratorQueue{
    public:
        void addToQueue(const ChunkCoord& mainChunkCoord, Chunk* mainChunk, Chunk* topChunk, Chunk* bottomChunk, Chunk* frontChunk, Chunk* backChunk, Chunk* leftChunk, Chunk* rightChunk);
        void generateSomeChunks(std::unordered_map<ChunkCoord, VBO>& chunkMap, const VoxelCoord focusPoint);
        void setMaxGenTime(int milliseconds);
    private:
        Timer mTimer;
        int mMaxGenTime = 10;
        VBOCreator mCreator;
        std::unordered_map<ChunkCoord, ChunkQueueThing> mQueue;
};
