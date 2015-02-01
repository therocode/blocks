class TestExtension01 : IExtension
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
        //int typeBlock = 9;
        //int typeEmpty = 0;
        //
        //if(coord.y != -1) 
        //    return;
        //
        //bool xFlipped = coord.x % 2 == 0;
        //bool zFlipped = coord.z % 2 == 0;

        //array<uint16> voxelData;
        //voxelData.resize(16*16*16);

        //int x = 0, y = 0, z = 0;
        //for(int x = 0; x < 16; x++) 
        //    for(int y = 0; y < 16; y++)
        //        for(int z = 0;z < 16; z++) 
        //        {
        //            int x_ = floor(16*sin((xFlipped ? x : (16 - x))/16.0));
        //            int z_ = floor(16*sin((zFlipped ? z : (16 - z))/16.0));
        //            
        //            int h = x_ < z_ ? x_ : z_;
        //            
        //            bool empty = y > h;
        //        
        //            setVoxelDataArray(voxelData, x, y, z, empty ? typeEmpty : typeBlock);
        //        }
        //        
        //chunk.setVoxelData(voxelData);
    }
}
