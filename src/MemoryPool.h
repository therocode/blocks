#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

#include <cstdlib>
#include <cstdio>

class MemoryPool{

    const static size_t CHUNK_HEADER_SIZE = sizeof(unsigned char*);

    struct memoryblock{
        void* nextBlock;
    };
    public:
    unsigned int maxSize;
    size_t chunkSize;
    unsigned int numChunks;

    int allocatedBlocks;
    bool allocated;

    unsigned char* memory;
    unsigned char*  head;
    memoryblock* lastBlockPointer;

    MemoryPool(size_t size){
        chunkSize = size;
    }

    //This will allocate a new block of memory and then set headers of all chunks.
    //[pointer to next][data...][pointer to next][data...]... or something.
    void AllocateBlock(int count){
        numChunks = count;
        //each memory chunk has a header that points to the next chunk. to get the actual data you point to pointer + chunk_header_size
        //size of one chunk
        size_t size = chunkSize + CHUNK_HEADER_SIZE;
        //Size of the whole block
        size_t blockSize = size * numChunks;
        printf("allocating block of %i chunks, each is %i bytes large. This is %iMB\n", numChunks, size, blockSize / 1024 / 1024);

        unsigned char* newMemory = (unsigned char*)malloc(blockSize);
    
        unsigned char* last = newMemory + blockSize;
        unsigned char* current= newMemory;
        //Set header pointers
        while(current < last){
            unsigned char* next = current + size;

            //make it a pointer to the array, so it's possible to set header pointer
            unsigned char** header = (unsigned char**)current;

            //Determine pointer to next chunk
            unsigned char*  nextchunk;
            if(next < last)
                nextchunk = next;
            else
                nextchunk = NULL;

            header[0] = nextchunk;
            current += size;
        }

        memory = newMemory;
        head = newMemory;

        printf("Memorypool block ceraetd\n");
    }

    //This will return the header of a block.
    unsigned char* GetNextChunk(unsigned char* block){
        //use as pointer pointer.
        unsigned char** header = (unsigned char**)block;
        return header[0];
    }

    //Thsi sets the nextchunk of a block
    void SetNextChunk(unsigned char* chunk, unsigned char* nextChunk){
        unsigned char** header = (unsigned char**)chunk;
        header[0] = nextChunk;
    }

    void* GetChunk(){
        if(!head){
            printf("no chunks left\n");
            exit(-1);
        }
        unsigned char* chunk = head;
        head = GetNextChunk(head);
        chunk += CHUNK_HEADER_SIZE;

        return chunk;
    }
    //when a chunk is deleted, it takes that chunk and puts it at the start of the chunk list. sets it to the head, and sets its next chunk to the previous head.
    void DeleteChunk(void* block){
        if(block == NULL){
            return;
        }
        
        //convertto memorypool chunk
        unsigned char* chunk = ((unsigned char*) block) - CHUNK_HEADER_SIZE;
        
        SetNextChunk(chunk, head);
        head = chunk;

        allocatedBlocks --;
    }
    void CreatePool(int count){
        if(allocated) return;
        AllocateBlock(count);
        allocatedBlocks = 0;
    }
    //Will delete all data.
    void DeletePool(){
    }
};

//This macro should be called in the header file of a class that should use the a memory pool.
#define DECLARE_MEMORYPOOL()\
public: \
    static MemoryPool* memoryPool; \
    static void* operator new(size_t size); \
    static void operator delete(void* ptr);\
     

#define IMPLEMENT_MEMORYPOOL(Type)\
    MemoryPool* Type::memoryPool = new MemoryPool(sizeof(Type));\
    void* Type::operator new(size_t size){\
        void* newmem = memoryPool->GetChunk();\
        return newmem;\
    }\
    void Type::operator delete(void* ptr){\
        memoryPool->DeleteChunk(ptr);\
    }\

#endif
