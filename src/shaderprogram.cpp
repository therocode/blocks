#include "shaderprogram.h"

ShaderProgram::ShaderProgram()
{
	mProgramCreated = false;
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
	if(!mProgramCreated)
	{
		mProgramID = glCreateProgram();
		mProgramCreated = true;
	}
	compile();
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
	glUniform1f(getUniformLocation(name), f);
}

void ShaderProgram::setUniform(std::string name, int   i)
{

	glUniform1i(getUniformLocation(name), i);
}

void ShaderProgram::setUniform(std::string name, glm::vec4 v)
{

	glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(v));
}

void ShaderProgram::setUniform(std::string name, glm::vec3 v)
{
	glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(v));
}

void ShaderProgram::setUniform(std::string name, glm::vec2 v)
{
	glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(v));
}

void ShaderProgram::setUniform(std::string name, glm::mat4 m)
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(m));
}
void ShaderProgram::setUniformMat4(std::string name, float* a)
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, a);
}

void ShaderProgram::setUniform(std::string name, glm::mat3 m)
{
	glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(m));
}

void ShaderProgram::compile()
{
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

GLint ShaderProgram::getUniformLocation(std::string name)
{
	if(!mProgramCreated)
	{
		return 0;
	}
    if(parameterCache.find(name) == parameterCache.end())
    {
        parameterCache.emplace(name, glGetUniformLocation(mProgramID, name.c_str()));
		printf("Cached shader uniform location %s\n", name.c_str());
    }
	return parameterCache.at(name);
}
