#pragma once
#include "blockstd.h"
#include "shader.h"
#include <fstream>
#include <sstream>

class ShaderProgram
{
	public:
		ShaderProgram();
		~ShaderProgram();
		///Sets shader paths.
		void setShaderPaths(std::string vertexPath, std::string fragmentPath);

		///Refreshes program, reloading and compiling all shaders.
		void refresh();
		
		void bind();
		void unbind();

		//At the moment only one texture!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! no!!! shadow!!!!! maps!!!!!! or !!!!!!!!!! normal Ma!!!ps.
		void setTexture(std::string name, GLint texture);
		//I guess this works.
		void setUniform(std::string name, float f);
		void setUniform(std::string name, int   i);
		void setUniform(std::string name, glm::vec4 v);
		void setUniform(std::string name, glm::vec3 v);
		void setUniform(std::string name, glm::mat4 m);
		void setUniform(std::string name, glm::mat3 m);

		GLint getAttribLocation(std::string name);
	private:
		void compile();
		GLint getUniformLocation(std::string name);
		GLuint mProgramID;
		//At the moment I just have one fragment and one vertex shader here.
		Shader mVertShader;
		Shader mFragShader;
};
