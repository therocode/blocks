#include "vbocreator.h"
#include <vector>
#include <iostream>

ChunkVBO VBOCreator::generateChunkVBO(const Chunk& chunk) const
{
	ChunkVBO vbo;


	const VoxelTypeArray& voxelTypes = chunk.getVoxelTypes();

	for(int i = 0; i < 1; i ++){
		for(int y = 0; y < 2; y++){
			float red = (float)(rand()%255) /255.f,
				  g = (float)(rand()%255) /255.f,
				  b = (float)(rand()%255) /255.f;

			float s = 0.5f;
			//Front

			Rectangle r;
			r.setColor(red,g,b);
			//front
			//	r.setColor(1,1,1);
			r.setPosition(0,i + s,i + y +  s, s); r.setUV(0, 1.0f, 1.0f);
			r.setPosition(1,i + -s,i + y +  s, s); r.setUV(1, 0.0f, 1.0f);
			r.setPosition(2,i + -s,i + y + -s, s); r.setUV(2, 0.0f, 0.0f);
			r.setPosition(3,i +  s,i + y + -s, s); r.setUV(3, 1.0f, 0.0f);
			r.calculateNormal();
			vbo.PushRectangle(r);

			//top
			r.setPosition(0, s, s, -s); r.setUV(0, 1.0f, 0.0f);
			r.setPosition(1,-s, s, -s); r.setUV(1, 0.0f, 0.0f);
			r.setPosition(2,-s, s, s); r.setUV(2, 0.0f, 1.0f);
			r.setPosition(3, s, s, s); r.setUV(3, 1.0f, 1.0f);
			r.calculateNormal();

//			r.setPosition(2, i +  s, i + y +  s, -s); r.setUV(0, 1.0f, 0.0f);
//			r.setPosition(2, i + -s, i + y +  s, -s); r.setUV(1, 0.0f, 0.0f);
//			r.setPosition(2, i + -s, i + y +  s, s); r.setUV(2, 0.0f, 1.0f);
//			r.setPosition(3, i +  s, i + y +  s, s); r.setUV(3, 1.0f, 1.0f);
//			r.calculateNormal();
			vbo.PushRectangle(r);

			//bottom
			//r.setColor(1,0,1);
			r.setPosition(0,i +  s,i + y  -s, s); r.setUV(0, 1.0f, 1.0f);
			r.setPosition(1,i + -s,i + y  -s, s); r.setUV(1, 0.0f, 1.0f);
			r.setPosition(2,i + -s,i + y  -s, -s); r.setUV(2, 0.0f, 0.0f);
			r.setPosition(3,i +  s,i + y  -s, -s); r.setUV(3, 1.0f, 0.0f);
			r.calculateNormal();
			vbo.PushRectangle(r);

			//back
			//r.setColor(0,0,1);
			r.setPosition(0,i +  s,i + y +  s, -s); r.setUV(0, 1.0f, 1.0f);
			r.setPosition(1,i +  s,i + y +  -s, -s); r.setUV(1, 1.0f, 0.0f);
			r.setPosition(2,i + -s,i + y +  -s, -s); r.setUV(2, 0.0f, 0.0f);
			r.setPosition(3,i +  -s,i + y +  s, -s); r.setUV(3, 0.0f, 1.0f);
			r.calculateNormal();
			vbo.PushRectangle(r);

			//right
			//r.setColor(0,1,1);
			r.setPosition(0,i + s,i + y +   s, -s); r.setUV(0, 1.0f, 0.0f);
			r.setPosition(1,i + s,i + y +   s,  s); r.setUV(1, 1.0f, 1.0f);
			r.setPosition(2,i + s,i + y +  -s,  s); r.setUV(2, 0.0f, 1.0f);
			r.setPosition(3,i + s,i + y +  -s, -s); r.setUV(3, 0.0f, 0.0f);
			r.calculateNormal();
			vbo.PushRectangle(r);

			//left
			//r.setColor(1,1,0);
			r.setPosition(0,i + -s,i + y +   s, -s); r.setUV(0, 1.0f, 0.0f);
			r.setPosition(1,i + -s,i + y +  -s, -s); r.setUV(1, 0.0f, 0.0f);
			r.setPosition(2,i + -s,i + y +  -s,  s); r.setUV(2, 0.0f, 1.0f);
			r.setPosition(3,i + -s,i + y +   s, s); r.setUV(3, 1.0f, 1.0f);
			r.calculateNormal();
			vbo.PushRectangle(r);
		}
	}

	//After stuff has been added, you have to update the gpu vbo data.
	vbo.UpdateVBO();

	return vbo;
}
