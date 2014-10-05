#include "../blockstd.hpp"
#include <vector>
///A debug renderer that draws stuff in immediate mode.
class DebugRenderer
{
	public:
		enum COLORS{
			WHITE,
			BLACK,
			RED,
			GREEN,
			BLUE, 
			ORANGE,
			YELLOW
		};
		void drawBox(float x, float y, float z, float sx, float sy, float sz, int color = WHITE);
		void drawCube(float x, float y, float z, float size, int color = WHITE);
		void drawLine(float startX, float startY, float startZ, float endX, float endY, float endZ, int color = WHITE);
		void drawPoint(float x, float y, float z, int color = WHITE);
		void performDrawing();
	private:
		void setColor(int color);
		struct DebugCube{
			//ALIGNED CODE IS IMPORTANT AS SHIT!
			float x,  y,  z ;
			float sx, sy, sz;
			int        color;
		};
		struct DebugLine{
			float sx, sy, sz;
			float ex, ey, ez;
			int        color;
		};
		struct DebugPoint{
			float x,  y,  z ;
			int        color;
		};
	std::vector<DebugCube> mCubes;
	std::vector<DebugLine> mLines;
	std::vector<DebugPoint> mPoints;
};
