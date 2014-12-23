class TestExtension02 : IExtension
{
    void setVoxelDataArray(array<uint16> &arr, uint x, uint y, uint z, uint16 type)
    {
        arr[y*16*16 + z*16 + x] = type;
    }

    uint16 getVoxelDataArray(array<uint16> &arr, uint x, uint y, uint z)
    {
        return arr[y*16*16 + z*16 + x];
    }

    void chunkInitiallyGenerated(const uint32 worldId, const ChunkCoord &in coord, Chunk &in chunk)
    {
        int typeBlock = 9;
        int typeEmpty = 0;

        array<uint16> voxelData = chunk.getVoxelData();

        for(int x = 0; x < 16; x++)
        {
            for(int y = 1; y < 15; y++) // We're not interested in the topmost and bottommost layers
            {
                for(int z = 0; z < 16; z++)
                {
                    uint16 typeBelow = getVoxelDataArray(voxelData, x, y-1, z);
                    uint16 type = getVoxelDataArray(voxelData, x, y, z);
                    uint16 typeAbove = getVoxelDataArray(voxelData, x, y+1, z);
                    
                    if(typeBelow != typeEmpty && type == typeEmpty && typeAbove == typeEmpty)
                    {
                        if(randomChance(0.5)) 
                        {
                            setVoxelDataArray(voxelData, x, y, z, typeBlock);
                        }
                    }
                }
            }
        }
        
        chunk.setVoxelData(voxelData);
    }
}
