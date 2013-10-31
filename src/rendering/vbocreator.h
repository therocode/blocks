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
    VBO generateChunkVBO(Chunk& chunk);
    VBO generateBoardVBO(const glm::vec2& dimensions) const;
private:
	///Helper function for vbo creation.
	inline void setRectData(Rectangle& r, float x, float y, float z, int face, float u, float v) const;
    static uint32_t totalTime;
    static uint32_t timesGenerated;
};
