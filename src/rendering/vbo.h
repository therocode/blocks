#pragma once
#include <vector>
#include <featherkit/rendering/opengl.h>
#include "blockstd.h"
#include "shaderprogram.h"

struct Vertex{
	float position[3] = {1,1,1};
	float color[3] = {1,1,1};
	float normal[3] = {1,1,1};
	float uv[2] = {1,1};
};
struct Triangle{
	Vertex vs[3];
};
struct Rectangle{
	Vertex vs[4];
	void setPosition(int i, float x, float y, float z){
		vs[i].position[0] = x;
		vs[i].position[1] = y;
		vs[i].position[2] = z;
	}
	void setAxisPosition(int axis, float x)
	{
		vs[0].position[axis] = x;
		vs[1].position[axis] = x;
		vs[2].position[axis] = x;
	}
	void setUV(int i, float u, float v){
		vs[i].uv[0] = u;
		vs[i].uv[1] = v;
	}
	void calculateNormal(){
		float* tPos = vs[0].position;
		glm::vec3 v0(tPos[0], tPos[1], tPos[2]);		

		tPos = vs[1].position;
		glm::vec3 v1(tPos[0], tPos[1], tPos[2]);

		tPos = vs[3].position;
		glm::vec3 v2(tPos[0], tPos[1], tPos[2]);

		v2 = v2 - v0;
		v1 = glm::cross(v2, v0 - v1);

		for(int i = 0; i < 4; i++){
			vs[i].normal[0] = v1.x;
			vs[i].normal[1] = v1.y;
			vs[i].normal[2] = v1.z;
		}	
	}
	void setColor(float r, float g, float b){
		for(Vertex &v : vs){
			v.color[0] = r;
			v.color[1] = g;
			v.color[2] = b;
		}	
	};
};
enum VBOIDs{
	VERTICES = 0, 
	INDICES
};

class VBO {
	public:
		VBO();
		~VBO();
		void PushTriangle(Triangle t);
		void PushRectangle(Rectangle r);
		void PushVertex(Vertex v);
		void Clear();
		void UpdateVBO();
		void DrawVBO();
		void DrawVBO(ShaderProgram& program);
		void SetDrawType(GLint type = GL_TRIANGLES);
		GLint GetDrawType();
		void DestroyVBO();
		void CreateVBO();
	private:
		GLuint mID[2];
		int mCurrentVBOByteSize;
		bool mVBOCreated;
		std::vector<Vertex> mvVertices;
		std::vector<int>	mvIndices;
		void BindBuffer();
		void UnbindBuffer();
		void DeleteBuffer();
		GLint mDrawType;
};
