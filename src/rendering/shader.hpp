#pragma once
#include "uniform.hpp"
#include "vertexattribute.hpp"
#include "buffer.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>

class GLSLException : public std::runtime_error 
{
    public:
        GLSLException(const std::string& message);
};

class Shader
{
    public:

        Shader();
        Shader(const Shader& other) = delete;
        Shader& operator=(const Shader& other) = delete;
        Shader(Shader&& other);
        Shader& operator=(Shader&& other);
        ~Shader();
        void setSource(const std::string& vertexSource, const std::string& fragmentSource);
        void activate() const;
        void deactivate() const;
        void setUniform(const std::string& name, UniformType type, const void* value) const;
        void setUniform(const std::string& name, UniformType type, int32_t count, const void* value) const;
        GLint getUniform(const std::string& name) const;
        void setVertexAttribute(const std::string& name, const uint32_t floatAmount, const float* data) const;
        void setVertexAttribute(const std::string& name, const uint32_t floatAmount, const Buffer& dataBuffer) const;
        void setInstanceAttribute(const std::string& name, const uint32_t floatAmount, const Buffer& dataBuffer, uint32_t divisor) const;
        GLint getVertexAttribute(const std::string& name) const;
        void compile();
    private:
        GLuint mProgramId;
        std::unordered_map<std::string, GLint> mUniformLocations;
        std::unordered_map<std::string, GLint> mVertexAttributeLocations;
        std::string mVertexSource;
        std::string mFragmentSource;
        GLuint mVertexShader;
        GLuint mFragmentShader;
        mutable std::vector<GLint> mEnabledVertexAttributes;
};
