#pragma once
#include "vbo.h"
#include "../world/chunk.h"
#include <chrono>

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
    VBO generateBoardVBO(const glm::vec2& dimensions) const;
private:
	///Helper function for vbo creation.
	inline void setRectData(Rectangle& r, float x, float y, float z, int face, float u, float v, float width, float height) const;
    static uint32_t totalTime;
    static uint32_t totalAmount;
    static uint32_t timesGenerated;
};
