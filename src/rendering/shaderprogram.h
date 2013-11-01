#pragma once
#include "../blockstd.h"
#include "shader.h"
#include <fstream>
#include <sstream>
#include <unordered_map>

class ShaderProgram
{
	public:
		ShaderProgram();
		~ShaderProgram();
		///Sets shader paths.
		void setShaderPaths(const std::string& vertexPath, const std::string& fragmentPath);

		///Refreshes program, reloading and compiling all shaders.
		void refresh();
		
		void bind();
		void unbind();

		//At the moment only one texture!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! no!!! shadow!!!!! maps!!!!!! or !!!!!!!!!! normal Ma!!!ps.
		void setTexture(const std::string& name, GLint texture);
		//I guess this works.
		void setUniform(const std::string& name, float f);
		void setUniform(const std::string& name, int   i);
		void setUniform(const std::string& name, const glm::vec4& v);
		void setUniform(const std::string& name, const glm::vec3& v);
		void setUniform(const std::string& name, const glm::vec2& v);
		void setUniform(const std::string& name, const glm::mat4& m);
		void setUniform(const std::string& name, const glm::mat3& m);
		
		void setUniformMat4(const std::string& name, float* a);

		GLint getAttribLocation(const std::string& name);
	private:
		void compile();
		bool   mProgramCreated;
		GLint getUniformLocation(const std::string& name);
		GLuint mProgramID;
		//At the moment I just have one fragment and one vertex shader here.
		Shader mVertShader;
		Shader mFragShader;

        std::unordered_map<std::string, GLint> parameterCache;
};
