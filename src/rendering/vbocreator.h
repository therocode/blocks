#pragma once
#include "vbo.h"
#include "../world/chunk.h"

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
    VBO generateChunkVBO(const Chunk& chunk) const;
    VBO generateBoardVBO(const glm::vec2& dimensions) const;
private:
	///Helper function for vbo creation.
	inline void setRectData(Rectangle& r, float x, float y, float z, int face, float u, float v) const;
};
