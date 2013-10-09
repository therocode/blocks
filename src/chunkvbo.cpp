#include "chunkvbo.h"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))
ChunkVBO::ChunkVBO():mCurrentChunkVBOByteSize(0),mChunkVBOCreated(false){
	mDrawType = GL_TRIANGLES;

}
ChunkVBO::~ChunkVBO(){

}
void ChunkVBO::PushTriangle(Triangle t){
	for(Vertex i : t.vs){
		PushVertex(i);
	}
}
void ChunkVBO::PushVertex(Vertex v){
	mvVertices.push_back(v);	
	mvIndices.push_back(mvVertices.size() - 1);
}
void ChunkVBO::Clear(){
	mvVertices.clear();
	mvIndices.clear();
}

void ChunkVBO::PushRectangle(Rectangle r){
	int si = mvVertices.size() ;
	for(Vertex v : r.vs){
		mvVertices.push_back(v);
	}
	mvIndices.push_back(si);
	mvIndices.push_back(si + 1);
	mvIndices.push_back(si + 2);

	mvIndices.push_back(si + 2);
	mvIndices.push_back(si + 3);
	mvIndices.push_back(si );
}

void ChunkVBO::UpdateVBO(){
	if(!mChunkVBOCreated){
		glGenBuffers(2, mID);
		mChunkVBOCreated = true;
	}
	mCurrentChunkVBOByteSize = sizeof(Vertex) * mvVertices.size();
	int idSize = sizeof(int) * mvIndices.size();

	BindBuffer();

	glBufferData(GL_ARRAY_BUFFER, mCurrentChunkVBOByteSize, mvVertices.data(), GL_DYNAMIC_DRAW); 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,idSize, mvIndices.data(), GL_DYNAMIC_DRAW);

	UnbindBuffer();
}

void ChunkVBO::DrawVBO(){
	BindBuffer();
	int stride = sizeof(Vertex);
#if 1
	//Use vertexpointer and stuff
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, stride, 0);
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, stride, BUFFER_OFFSET(3 * sizeof(float)));
 
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, stride, BUFFER_OFFSET(6 * sizeof(float)));

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, stride, BUFFER_OFFSET(9 * sizeof(float)));

	glDrawElements(mDrawType, mvIndices.size(), GL_UNSIGNED_INT, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#else
	for(int i = 0; i< 4; i++){
		glEnableVertexAttribArray(i);
	}
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride , 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride , BUFFER_OFFSET(3 * sizeof(float)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride , BUFFER_OFFSET(6 * sizeof(float)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride , BUFFER_OFFSET(9 * sizeof(float)));
	glDrawElements(mDrawType, mvIndices.size(), GL_UNSIGNED_INT, 0);
	for(int i = 0; i< 4; i++){
		glDisableVertexAttribArray(i);
	}
#endif
	UnbindBuffer();
}

void ChunkVBO::BindBuffer(){
	if(mChunkVBOCreated){
		glBindBuffer(GL_ARRAY_BUFFER, mID[VERTICES]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID[INDICES]);
	}
}
void ChunkVBO::UnbindBuffer(){
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void ChunkVBO::DeleteBuffer(){
	if(mChunkVBOCreated){
		glDeleteBuffers(2, mID);
		mChunkVBOCreated = false;
	}
}
void ChunkVBO::SetDrawType(GLint type){
	mDrawType = type;
}
GLint ChunkVBO::GetDrawType(){
	return mDrawType;
}
