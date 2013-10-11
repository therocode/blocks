#include "blockstd.h"
#include <fstream>
#include <sstream>

using namespace std;
class Shader
{
	public:
		Shader();
		~Shader();
		///Loads shader from file.
		void loadFromFile(std::string path, GLuint shaderType);
		///If shader has been edited, it can be reloaded with this.
		void refreshShader();
		///unloads shader from gl memory. good to use after shader program has been compiled.
		void unloadShader();
		///wow this totally compiles the shader.
		void compileShader();
		GLuint getType() const {return mShaderType;}
		GLuint getID()   const {return mShaderID;}
	private:
		void createShader();
		void destroyShader();
		GLuint mShaderType;
		GLuint mShaderID;
		std::string mShaderPath;
		std::string mShaderSource;
};
