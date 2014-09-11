#include "debugrenderer.hpp"
#include <GL/gl.h>

void DebugRenderer::drawBox(float x, float y, float z, float sx, float sy, float sz, int color)
{
	DebugCube c;
	c.x = x;
	c.y = y;
	c.z = z;
	c.sx = sx;
	c.sy = sy;
	c.sz = sz;
	c.color = color;
	mCubes.push_back(c);
}

void DebugRenderer::drawCube(float x, float y, float z, float size, int color )
{
	drawBox(x, y, z, size, size, size, color);
}

void DebugRenderer::drawLine(float startX, float startY, float startZ, float endX, float endY, float endZ, int color )
{
	DebugLine l;
	l.sx = startX;
	l.sy = startY;
	l.sz = startZ;
	l.ex = endX;
	l.ey = endY;
	l.ez = endZ;
	l.color = color;
	mLines.push_back(l);
}

void DebugRenderer::drawPoint(float x, float y, float z, int color)
{
	DebugPoint p;
	p.x = x;
	p.y = y;
	p.z = z;
	p.color = color;
	mPoints.push_back(p);
}

void DebugRenderer::performDrawing()
{
	// glDisable(GL_DEPTH_TEST);
	
	for(DebugCube c : mCubes)
	{
		setColor(c.color);
		glBegin(GL_LINE_STRIP);
		float sx = c.sx * 0.5f;
		float sy = c.sy * 0.5f;
		float sz = c.sz * 0.5f;
		glm::vec3 v = glm::vec3(c.x, c.y, c.z);
		//top
		glm::vec3 p = v + glm::vec3(sx, sy, sz);
		glVertex3f(p.x, p.y, p.z);
		
		p = v + glm::vec3(sx, sy, -sz);
		glVertex3f(p.x, p.y, p.z);

		p = v + glm::vec3(-sx, sy, -sz);
		glVertex3f(p.x, p.y, p.z);

		p = v + glm::vec3(-sx, sy, sz);
		glVertex3f(p.x, p.y, p.z);
		
		//bottom
		p = v + glm::vec3(-sx, -sy, -sz);
		glVertex3f(p.x, p.y, p.z);
		
		p = v + glm::vec3(sx, -sy, -sz);
		glVertex3f(p.x, p.y, p.z);

		p = v + glm::vec3(sx, -sy, sz);
		glVertex3f(p.x, p.y, p.z);

		p = v + glm::vec3(-sx, -sy, sz);
		glVertex3f(p.x, p.y, p.z);
		
		//left
		p = v + glm::vec3(-sx, sy, -sz);
		glVertex3f(p.x, p.y, p.z);
		
		p = v + glm::vec3(-sx, -sy, -sz);
		glVertex3f(p.x, p.y, p.z);
		
		p = v + glm::vec3(-sx, -sy, sz);
		glVertex3f(p.x, p.y, p.z);

		p = v + glm::vec3(-sx, sy, sz);
		glVertex3f(p.x, p.y, p.z);
		
		//right
		p = v + glm::vec3(sx, -sy, -sz);
		glVertex3f(p.x, p.y, p.z);
		
		p = v + glm::vec3(sx, sy, -sz);
		glVertex3f(p.x, p.y, p.z);
		
		p = v + glm::vec3(sx, sy, sz);
		glVertex3f(p.x, p.y, p.z);
		
		p = v + glm::vec3(sx, -sy, sz);
		glVertex3f(p.x, p.y, p.z);
		
		//-z
		p = v + glm::vec3(sx, sy, -sz);
		glVertex3f(p.x, p.y, p.z);
		
		p = v + glm::vec3(sx, -sy, -sz);
		glVertex3f(p.x, p.y, p.z);
		
		p = v + glm::vec3(-sx, -sy, -sz);
		glVertex3f(p.x, p.y, p.z);
		
		p = v + glm::vec3(-sx, sy, -sz);
		glVertex3f(p.x, p.y, p.z);
		
		//z
		p = v + glm::vec3(sx, sy, sz);
		glVertex3f(p.x, p.y, p.z);
		
		p = v + glm::vec3(-sx, sy, sz);
		glVertex3f(p.x, p.y, p.z);
		
		p = v + glm::vec3(-sx, -sy, sz);
		glVertex3f(p.x, p.y, p.z);
		
		p = v + glm::vec3(sx, -sy, sz);
		glVertex3f(p.x, p.y, p.z);
		glEnd();
	}
	
	glBegin(GL_LINES);
	for(DebugLine l : mLines)
	{
		setColor(l.color);
		glVertex3f(l.sx, l.sy, l.sz);
		glVertex3f(l.ex, l.ey, l.ez);
	}
	glEnd();
	
	glBegin(GL_POINTS);
	for(DebugPoint l : mPoints)
	{
		setColor(l.color);
		glVertex3f(l.x, l.y, l.z);
	}
	glEnd();
	
	mCubes.clear();
	mLines.clear();
	mPoints.clear();
	
	glEnable(GL_DEPTH_TEST);
}
void DebugRenderer::setColor(int color)
{
	switch(color)
	{
		case BLACK:
			glColor3f(0,0,0);
			break;
		case WHITE:
			glColor3f(1,1,1);
			break;
		case RED:
			glColor3f(1,0,0);
			break;
		case GREEN:
			glColor3f(0,1,0);
			break;
		case BLUE:
			glColor3f(0,0,1);
			break;
		case ORANGE:
			glColor3f(1,0.5f,0);
			break;
		case YELLOW:
			glColor3f(1,1,0);
			break;
		default:
			glColor3f(1,1,1);
			break;
	}
}
