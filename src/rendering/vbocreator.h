#pragma once
#include "blockstd.h"
//#include "vbo.h"
#include "../world/chunk.h"
#include <chrono>
#include "newvbo.h"

/* Vertex with bounds.*/
struct ChunkVertex{
    float vert[3] ;
    float color[3];
    float normal[3];
    float uv[2];
    float bounds[4];
    void reset(){
        vert[0] = vert[1] = vert[2] = 0;
        color[0] = color[1] = color[2] = 1.f;
        normal[0] = normal[1] = normal[2] = 1.f;
        uv[0] = uv[1] = 0.f;
    }
};

struct ChunkTriangle {
    ChunkVertex vs[3];
    void setPosition(int i, float x, float y, float z){
        if(i < 0 || i > 2) {
            return;
        }
        vs[i].vert[0] = x;
        vs[i].vert[1] = y;
        vs[i].vert[2] = z;
    }
    void setUV(int i, float u, float v){
        if(i < 0 || i > 2) {
            return;
        }
        vs[i].uv[0] = u;
        vs[i].uv[1] = v;
    }
    void calculateNormal(){
        float* tPos = vs[0].vert;
        glm::vec3 v0(tPos[0], tPos[1], tPos[2]);

        tPos = vs[1].vert;
        glm::vec3 v1(tPos[0], tPos[1], tPos[2]);

        tPos = vs[2].vert;
        glm::vec3 v2(tPos[0], tPos[1], tPos[2]);

        v2 = v2 - v0;
        v1 = glm::cross(v2, v0 - v1);

        for(int i = 0; i < 3; i++){
            vs[i].normal[0] = v1.x;
            vs[i].normal[1] = v1.y;
            vs[i].normal[2] = v1.z;
        }
    }
    void pushIndicesIntoVBO(VBO& target){
        int startID = target.getVertexCount();
        target.pushIndex(startID);
        target.pushIndex(startID + 1);
        target.pushIndex(startID + 2);
    }
};

/*chunk rectangle*/
struct ChunkRect{
    ChunkVertex vs[4];
    void setPosition(int i, float x, float y, float z){
        vs[i].vert[0] = x;
        vs[i].vert[1] = y;
        vs[i].vert[2] = z;
    }
    void setAxisPosition(int axis, float x)
    {
        vs[0].vert[axis] = x;
        vs[1].vert[axis] = x;
        vs[2].vert[axis] = x;
    }
    void setUV(int i, float u, float v){
        vs[i].uv[0] = u;
        vs[i].uv[1] = v;
    }
    void calculateNormal(){
        float* tPos = vs[0].vert;
        glm::vec3 v0(tPos[0], tPos[1], tPos[2]);

        tPos = vs[1].vert;
        glm::vec3 v1(tPos[0], tPos[1], tPos[2]);

        tPos = vs[3].vert;
        glm::vec3 v2(tPos[0], tPos[1], tPos[2]);

        v2 = v2 - v0;
        v1 = glm::cross(v2, v0 - v1);

        for(int i = 0; i < 4; i++){
            vs[i].normal[0] = v1.x;
            vs[i].normal[1] = v1.y;
            vs[i].normal[2] = v1.z;
        }
    }
    void reset(){
        for(auto &v : vs)v.reset();
    }
    void setColor(float r, float g, float b){
        for(ChunkVertex &v : vs){
            v.color[0] = r;
            v.color[1] = g;
            v.color[2] = b;
        }
    }
    void setBounds(float sx, float sy, float ex, float ey){
        for(ChunkVertex &v : vs){
            v.bounds[0] = sx;
            v.bounds[1] = sy;
            v.bounds[2] = ex;
            v.bounds[3] = ey;
        }
    }
    void pushIndicesIntoVBO(VBO& target){
        int startID = target.getVertexCount();
        target.pushIndex(startID);
        target.pushIndex(startID + 1);
        target.pushIndex(startID + 2);

        target.pushIndex(startID + 2);
        target.pushIndex(startID + 3);
        target.pushIndex(startID);
    }
};

/*Billboard vertex and rect*/
struct BillboardVertex{
	float vert[3] ;
    float color[3];
    float normal[3];
    float uv[2];
    void reset(){
        vert[0] = vert[1] = vert[2] = 0;
        color[0] = color[1] = color[2] = 1.f;
        normal[0] = normal[1] = normal[2] = 1.f;
        uv[0] = uv[1] = 0.f;
    }
};
struct BillboardRect{
    BillboardVertex vs[4];
    void setPosition(int i, float x, float y, float z){
        vs[i].vert[0] = x;
        vs[i].vert[1] = y;
        vs[i].vert[2] = z;
    }
    void setAxisPosition(int axis, float x)
    {
        vs[0].vert[axis] = x;
        vs[1].vert[axis] = x;
        vs[2].vert[axis] = x;
    }
    void setUV(int i, float u, float v){
        vs[i].uv[0] = u;
        vs[i].uv[1] = v;
    }
    void calculateNormal(){
        float* tPos = vs[0].vert;
        glm::vec3 v0(tPos[0], tPos[1], tPos[2]);

        tPos = vs[1].vert;
        glm::vec3 v1(tPos[0], tPos[1], tPos[2]);

        tPos = vs[3].vert;
        glm::vec3 v2(tPos[0], tPos[1], tPos[2]);

        v2 = v2 - v0;
        v1 = glm::cross(v2, v0 - v1);

        for(int i = 0; i < 4; i++){
            vs[i].normal[0] = v1.x;
            vs[i].normal[1] = v1.y;
            vs[i].normal[2] = v1.z;
        }
    }
    void reset(){
        for(auto &v : vs)v.reset();
    }
    void setColor(float r, float g, float b){
        for(auto &v : vs){
            v.color[0] = r;
            v.color[1] = g;
            v.color[2] = b;
        }
    }

    void pushIndicesIntoVBO(VBO& target){
        int startID = target.getVertexCount();
        target.pushIndex(startID);
        target.pushIndex(startID + 1);
        target.pushIndex(startID + 2);

        target.pushIndex(startID + 2);
        target.pushIndex(startID + 3);
        target.pushIndex(startID);
    }
};


class VBOCreator
{
	enum FACES{
		BOTTOM,
		LEFT,
		TOP,
		RIGHT,
		FRONT,
		BACK,
		CENTER//Center is a front in the center.
	};
    public:
    VBOCreator();
    VBO generateChunkVBO(const ChunkCoordinate& coord, const VoxelTypeData& voxelTypeData);
    VBO generateChunkVBO(Chunk* mainChunk, Chunk* topChunk, Chunk* bottomChunk, Chunk* frontChunk, Chunk* backChunk, Chunk* leftChunk, Chunk* rightChunk);
    VBO generateBoardVBO(const glm::vec2& dimensions) const;
private:
	///Helper function for vbo creation.
	//inline void setRectData(Rectangle& r, float x, float y, float z, int face, float u, float v, float width, float height) const;
    inline void setChunkRectData(ChunkRect& r,float x, float y, float z, int face, float u, float v, float width, float height) const;
    static uint32_t totalTime;
    static uint32_t totalAmount;
    static uint32_t timesGenerated;
};