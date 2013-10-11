#include "shader.h"

Shader::Shader()
{
	mShaderType = 0;
	mShaderID   = 0;
}

Shader::~Shader()
{
}
void Shader::loadFromFile(std::string path, GLuint shaderType)
{
	mShaderType = shaderType;
	mShaderPath = path;
	createShader();
	refreshShader();
}

void Shader::refreshShader()
{
	if(mShaderID != 0)
	{
		std::ifstream file;
		file.open(mShaderPath.c_str());
		if(!file)
		{
			return;
		}
		std::stringstream stream;
		stream << file.rdbuf();
		file.close();

		mShaderSource = stream.str();

		const char* fileData = mShaderSource.c_str();

		glShaderSource(mShaderID, 1, &fileData, NULL);
	}
	compileShader();
}

void Shader::compileShader()
{ 
	glCompileShader(mShaderID);
	GLint error = GL_TRUE;
	glGetShaderiv(mShaderType, GL_COMPILE_STATUS, &error);
	if(error == GL_FALSE){
		GLint infoLogLength;
		glGetShaderiv(mShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfo = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(mShaderID, infoLogLength, NULL, strInfo);
		const char* shaderType = NULL;
		switch(mShaderType){
			case GL_VERTEX_SHADER: shaderType ="vertex";break;
			case GL_FRAGMENT_SHADER: shaderType = "fragment";break;
		} 
		fprintf(stderr, "%s shader failed to compile: %s\n", shaderType, strInfo);
		delete[] strInfo;
	}
}

void Shader::unloadShader()
{
	destroyShader();
}

void Shader::createShader()
{
	if(mShaderID == 0)
	{
		mShaderID = glCreateShader(mShaderType);
	}
}
void Shader::destroyShader()
{
	if(mShaderID != 0)
	{
		glDeleteShader(mShaderType);
		mShaderID = 0;
	}
}
