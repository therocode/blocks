float rootChance = 0.01;
float leafChance = 0.5;
uint16 rootType = 6;
uint16 airType = 0;
uint16 leafType = 4;
int framesPerGrowth = 300;

void setVoxelDataArray(array<uint16> &arr, uint x, uint y, uint z, uint16 type)
{
	arr[y*16*16 + z*16 + x] = type;
}

uint16 getVoxelDataArray(array<uint16> &arr, uint x, uint y, uint z)
{
	return arr[y*16*16 + z*16 + x];
}

int getVoxelDataArrayIndex(uint x, uint y, uint z)
{
	return y*16*16 + z*16 + x;
}

void applyLeafMods(array<uint16> &arr, array<int> leafMods)
{
	for(int i = 0; i < leafMods.length(); i++)
		arr[leafMods[i]] = leafType;
}

void placeRoots(Chunk &chunk) 
{
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
				
				if(typeBelow != 0 && type == 0 && typeAbove == 0 && typeBelow != rootType)
				{
					if(randomChance(rootChance)) 
					{
						setVoxelDataArray(voxelData, x, y, z, rootType);
					}
				}
			}
		}
	}
	
	chunk.setVoxelData(voxelData);
}

bool hasFullStem(array<uint16> &arr, uint x, uint y, uint z)
{
	if(y < 2) 
		return false;
		
	int t0 = getVoxelDataArray(arr, x, y, z);
	int t1 = getVoxelDataArray(arr, x, y-1, z);
	int t2 = getVoxelDataArray(arr, x, y-2, z);
	
	return t0 == rootType && t1 == rootType && t2 == rootType;
}

void addStem(array<uint16> &arr, uint x, uint y, uint z)
{
	int type = getVoxelDataArray(arr, x, y, z);
	
	if(type == airType)
	{
		setVoxelDataArray(arr, x, y, z, rootType);
	}
}

void addFirstLeaves(array<uint16> &arr, uint x, uint y, uint z)
{
	int type = getVoxelDataArray(arr, x, y, z);
	
	if(type == airType)
	{
		setVoxelDataArray(arr, x, y, z, leafType);
	}
}

void expandLeaves(array<uint16> &arr, array<int> &leafMods, uint x, uint y, uint z)
{
	int typeAbove = getVoxelDataArray(arr, x, y+1, z);
	int typeLeft = getVoxelDataArray(arr, x-1, y, z);
	int typeRight = getVoxelDataArray(arr, x+1, y, z);
	int typeDown = getVoxelDataArray(arr, x, y, z-1);
	int typeUp = getVoxelDataArray(arr, x, y, z+1);

	if(typeAbove == airType && randomChance(leafChance))
		leafMods.insertLast(getVoxelDataArrayIndex(x, y+1, z));
	if(typeLeft == airType && randomChance(leafChance))
		leafMods.insertLast(getVoxelDataArrayIndex(x-1, y, z));
	if(typeRight == airType && randomChance(leafChance))
		leafMods.insertLast(getVoxelDataArrayIndex(x+1, y, z));
	if(typeDown == airType && randomChance(leafChance))
		leafMods.insertLast(getVoxelDataArrayIndex(x, y, z-1));
	if(typeUp == airType && randomChance(leafChance))
		leafMods.insertLast(getVoxelDataArrayIndex(x, y, z+1));
}

void grow(Chunk &chunk, const uint64 frameNumberDelta)
{
	array<uint16> voxelData = chunk.getVoxelData();
	
	int growths = frameNumberDelta/framesPerGrowth;
	
	array<int> leafMods;
		
	for(int i = 0; i < growths; i++)
	{
		for(int x = 1; x < 15; x++)
		{
			for(int y = 1; y < 15; y++) // We're not interested in the topmost and bottommost layers
			{
				for(int z = 1; z < 15; z++)
				{
					uint16 type = getVoxelDataArray(voxelData, x, y, z);
					
					if(type == rootType)
					{
						if(hasFullStem(voxelData, x, y, z))
						{
							addFirstLeaves(voxelData, x, y+1, z);
						} else 
						{
							addStem(voxelData, x, y+1, z);
						}
					}
					
					if(type == leafType)
					{
						expandLeaves(voxelData, leafMods, x, y, z);
					}
				}
			}
		}
		
		applyLeafMods(voxelData, leafMods);
	}

	chunk.setVoxelData(voxelData);
}

void chunkInitiallyGenerated(const uint32 worldId, const ChunkCoord &in coord, Chunk &in chunk)
{
	//placeRoots(chunk);
}

void chunkCandidate(const uint32 worldId, const ChunkCoord &in coord, Chunk &in chunk, const uint64 frameNumberDelta)
{
    //grow(chunk, frameNumberDelta);
}
