#include "shaderprogram.h"

ShaderProgram::ShaderProgram()
{

}
ShaderProgram::~ShaderProgram()
{

}
void ShaderProgram::setShaderPaths(std::string vertexPath, std::string fragmentPath)
{
	mVertShader.loadFromFile(vertexPath,   GL_VERTEX_SHADER);
	mFragShader.loadFromFile(fragmentPath, GL_FRAGMENT_SHADER);
	refresh();
}

void ShaderProgram::refresh()
{
	mVertShader.refreshShader();
	mFragShader.refreshShader();
	mProgramID = glCreateProgram();
	glAttachShader(mProgramID, mVertShader.getID());
	glAttachShader(mProgramID, mFragShader.getID());

	glLinkProgram(mProgramID);
	GLint status = GL_TRUE;
	glGetProgramiv(mProgramID, GL_LINK_STATUS, &status);
	if(status == GL_FALSE){
		GLint infoLogLength;
		glGetProgramiv(mProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfo = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(mProgramID, infoLogLength, NULL, strInfo);
		fprintf(stderr, "program failed to link: %s\n",  strInfo);
		delete[] strInfo;

	}else{
        printf("Program Linked and compiled.\n");

        parameterCache.clear();
        }
}
void ShaderProgram::bind()
{
	glUseProgram(mProgramID);
}
void ShaderProgram::unbind()
{
	glUseProgram(0);
}

GLint ShaderProgram::getAttribLocation(std::string name)
{
    if(parameterCache.find(name) == parameterCache.end())
    {
        parameterCache.emplace(name, glGetAttribLocation(mProgramID, name.c_str()));
    }
	
	return parameterCache.at(name);
}	
void ShaderProgram::setTexture(std::string name, GLint texture)
{
	setUniform(name, (int)0);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texture);
}
void ShaderProgram::setUniform(std::string name, float f)
{
    if(parameterCache.find(name) == parameterCache.end())
    {
        parameterCache.emplace(name, glGetUniformLocation(mProgramID, name.c_str()));
    }

	glUniform1f(parameterCache.at(name), f);
}

void ShaderProgram::setUniform(std::string name, int   i)
{
    if(parameterCache.find(name) == parameterCache.end())
    {
        parameterCache.emplace(name, glGetUniformLocation(mProgramID, name.c_str()));
    }

	glUniform1i(parameterCache.at(name), i);
}

void ShaderProgram::setUniform(std::string name, glm::vec4 v)
{
    if(parameterCache.find(name) == parameterCache.end())
    {
        parameterCache.emplace(name, glGetUniformLocation(mProgramID, name.c_str()));
    }

	glUniform4fv(parameterCache.at(name), 1, glm::value_ptr(v));
}

void ShaderProgram::setUniform(std::string name, glm::vec3 v)
{
    if(parameterCache.find(name) == parameterCache.end())
    {
        parameterCache.emplace(name, glGetUniformLocation(mProgramID, name.c_str()));
    }

	glUniform3fv(parameterCache.at(name), 1, glm::value_ptr(v));
}

void ShaderProgram::setUniform(std::string name, glm::mat4 m)
{
    if(parameterCache.find(name) == parameterCache.end())
    {
        parameterCache.emplace(name, glGetUniformLocation(mProgramID, name.c_str()));
    }

	glUniformMatrix4fv(parameterCache.at(name), 1, GL_FALSE, glm::value_ptr(m));
}

void ShaderProgram::setUniform(std::string name, glm::mat3 m)
{
    if(parameterCache.find(name) == parameterCache.end())
    {
        parameterCache.emplace(name, glGetUniformLocation(mProgramID, name.c_str()));
    }

	glUniformMatrix3fv(parameterCache.at(name), 1, GL_FALSE, glm::value_ptr(m));
}

void ShaderProgram::compile()
{

}

GLint ShaderProgram::getUniformLocation(std::string name)
{
    if(parameterCache.find(name) == parameterCache.end())
    {
        parameterCache.emplace(name, glGetUniformLocation(mProgramID, name.c_str()));
    }
	return parameterCache.at(name);
}
