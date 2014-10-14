#include "chunkgeneratorqueue.hpp"

void ChunkGeneratorQueue::addToQueue(const ChunkCoord& mainChunkCoord, Chunk* mainChunk, Chunk* topChunk, Chunk* bottomChunk, Chunk* frontChunk, Chunk* backChunk, Chunk* leftChunk, Chunk* rightChunk){
    auto main = mQueue.find(mainChunkCoord);
    ChunkQueueThing t;
    t.mainChunkCoord = mainChunkCoord;
    t.mainChunk = mainChunk;
    t.topChunk  = topChunk;
    t.bottomChunk = bottomChunk;
    t.frontChunk = frontChunk;
    t.backChunk = backChunk;
    t.leftChunk = leftChunk;
    t.rightChunk = rightChunk;
    if(main == mQueue.end()){
        mQueue.emplace(mainChunkCoord, t);
    }else{
        mQueue.erase(main);
        mQueue.emplace(mainChunkCoord, t);
    }
}
void ChunkGeneratorQueue::generateSomeChunks(std::unordered_map<ChunkCoord, VBO>& chunkMap, const VoxelCoord focusPoint){
    mTimer.start();
    while(mTimer.getTime() < mMaxGenTime){
        auto iterator = mQueue.begin(); 
        if(iterator == mQueue.end()){
            break;
        }
        auto closestChunk = iterator;
        float minDist = 999999999.f;
        for(;iterator != mQueue.end(); iterator ++){
            VoxelCoord c = iterator->first;
            c *= chunkWidth;
            float d = glm::length((glm::vec3)(c - focusPoint));
            if(d < minDist){
                minDist = d;
                closestChunk = iterator;
            }
        }
        if(closestChunk != mQueue.end()){
            auto vboEntry = chunkMap.find(closestChunk->first);
            auto m = closestChunk->second;

            if(vboEntry == chunkMap.end())
            {
                chunkMap.emplace(m.mainChunkCoord, mCreator.generateChunkVBO(m.mainChunkCoord, m.mainChunk, m.topChunk, m.bottomChunk, m.frontChunk, m.backChunk, m.leftChunk, m.rightChunk));
            }
            else
            {
                chunkMap.at(closestChunk->first).destroyBuffers();
                chunkMap.erase(closestChunk->first);
                chunkMap.emplace(m.mainChunkCoord, mCreator.generateChunkVBO(m.mainChunkCoord, m.mainChunk, m.topChunk, m.bottomChunk, m.frontChunk, m.backChunk, m.leftChunk, m.rightChunk));
            }
            mQueue.erase(closestChunk->first);
        }
    }
    mTimer.stop();
}
void ChunkGeneratorQueue::setMaxGenTime(int milliseconds){
    mMaxGenTime = milliseconds;
}
