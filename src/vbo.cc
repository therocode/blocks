#include "vbo.h"
#include <iostream>

#define BUFFER_OFFSET(i) ((char*)NULL + (i))
namespace fys{
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
		if(!mVBOCreated){
			glGenBuffers(2, mID);
			mVBOCreated = true;
		}
		mCurrentVBOByteSize = sizeof(Vertex) * mvVertices.size();
		int idSize = sizeof(int) * mvIndices.size();

		BindBuffer();

		glBufferData(GL_ARRAY_BUFFER, mCurrentVBOByteSize, mvVertices.data(), GL_DYNAMIC_DRAW); 
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,idSize, mvIndices.data(), GL_DYNAMIC_DRAW);

		UnbindBuffer();
	}

	void VBO::DrawVBO(){
		BindBuffer();
		
		int stride = sizeof(Vertex);
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
}
