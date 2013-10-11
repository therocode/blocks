#pragma once
#include "vbo.h"
#include "chunk.h"

class VBOCreator
{
	enum FACES{
		BOTTOM,
		LEFT,
		TOP,
		RIGHT,
		FRONT,
		BACK
	};
    public:
    VBO generateChunkVBO(const Chunk& chunk) const;
private:
	///Helper function for vbo creation.
	inline void setRectData(Rectangle& r, float x, float y, float z, int face, float u, float v) const;
};
