#pragma once
#include "chunkdeliverer.h"

class Landscape
{
    public:
        void setChunkDeliverer(ChunkDeliverer* del);
    private:
        ChunkDeliverer* chunkDeliverer;
};
