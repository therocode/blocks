#include "vbocreator.h"
#include <vector>
#include <iostream>

ChunkVBO VBOCreator::generateChunkVBO(const Chunk& chunk) const
{
    ChunkVBO vbo;


    const VoxelTypeArray& voxelTypes = chunk.getVoxelTypes();

	for(int i = 0; i < 10; i ++){
		//haha nothing.
		Rectangle r;
		r.setPosition(0, i, i+1, 0);
		r.setPosition(1, i, i, 0);
		r.setPosition(2, i+1, i, 0);
		r.setPosition(3, i+ 1, i+1, 0);
		r.calculateNormal();
		vbo.PushRectangle(r);

	}

	//After stuff has been added, you have to update the gpu vbo data.
	vbo.UpdateVBO();

    return vbo;
}
