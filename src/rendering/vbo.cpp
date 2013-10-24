#include "vbo.h"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))
VBO::VBO():mCurrentVBOByteSize(0),mVBOCreated(false){
	mDrawType = GL_TRIANGLES;

}
VBO::~VBO(){
}
void VBO::PushTriangle(Triangle t){
	for(Vertex i : t.vs){
		PushVertex(i);
	}
}
void VBO::PushVertex(Vertex v){
	mvVertices.push_back(v);	
	mvIndices.push_back(mvVertices.size() - 1);
}
void VBO::Clear(){
	mvVertices.clear();
	mvIndices.clear();
}

void VBO::PushRectangle(Rectangle r){
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

void VBO::UpdateVBO(){
	CreateVBO();
	mCurrentVBOByteSize = sizeof(Vertex) * mvVertices.size();
	int idSize = sizeof(int) * mvIndices.size();

	BindBuffer();

	glBufferData(GL_ARRAY_BUFFER, mCurrentVBOByteSize, mvVertices.data(), GL_DYNAMIC_DRAW); 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,idSize, mvIndices.data(), GL_DYNAMIC_DRAW);

	UnbindBuffer();
}
void VBO::DestroyVBO()
{
	Clear();
	if(mVBOCreated)
	{
		glDeleteBuffers(2, mID);
		mVBOCreated = false;
	}
}
void VBO::CreateVBO()
{
	if(!mVBOCreated){
		glGenBuffers(2, mID);
		mVBOCreated = true;
	}
}
void VBO::DrawVBO(){
	BindBuffer();
	int stride = sizeof(Vertex);
#if 0 
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
void VBO::DrawVBO(ShaderProgram& program)
{
	BindBuffer();
	int stride = sizeof(Vertex);
	
	GLint i = program.getAttribLocation("vert");
	glEnableVertexAttribArray(i);
	glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, stride , 0);
	

	i = program.getAttribLocation("normal");
	glEnableVertexAttribArray(i);
	glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, stride , BUFFER_OFFSET(6 * sizeof(float)));

	i = program.getAttribLocation("color");
	glEnableVertexAttribArray(i);
	glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, stride , BUFFER_OFFSET(3 * sizeof(float)));
	
	i = program.getAttribLocation("uv");
	glEnableVertexAttribArray(i);
	glVertexAttribPointer(i, 2, GL_FLOAT, GL_FALSE, stride , BUFFER_OFFSET(9 * sizeof(float)));


	glDrawElements(mDrawType, mvIndices.size(), GL_UNSIGNED_INT, 0);

	UnbindBuffer();
}

void VBO::BindBuffer(){
	if(mVBOCreated){
		glBindBuffer(GL_ARRAY_BUFFER, mID[VERTICES]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID[INDICES]);
	}
}
void VBO::UnbindBuffer(){
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void VBO::DeleteBuffer(){
	if(mVBOCreated){
		glDeleteBuffers(2, mID);
		mVBOCreated = false;
	}
}
void VBO::SetDrawType(GLint type){
	mDrawType = type;
}
GLint VBO::GetDrawType(){
	return mDrawType;
}
