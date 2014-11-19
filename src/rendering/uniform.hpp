#pragma once
#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#include "opengl.hpp"

enum UniformType{NO_TYPE, FLOAT, VEC2, VEC3, VEC4, MAT2X2, MAT4X4, TEXTURE};

struct Uniform
{
    Uniform();
    Uniform(const std::string& name, UniformType t, float val);
    Uniform(const std::string& name, UniformType t, glm::vec2 val);
    Uniform(const std::string& name, UniformType t, glm::vec3 val);
    Uniform(const std::string& name, UniformType t, glm::vec4 val);
    Uniform(const std::string& name, UniformType t, glm::mat2x2 val);
    Uniform(const std::string& name, UniformType t, glm::mat4x4 val);
    Uniform(const std::string& name, UniformType t, GLuint val);
    Uniform(const Uniform& other);

    std::string mName;
    UniformType mType;
    union
    {
        float mFloatVal;      
        float mVec2Val[2];    
        float mVec3Val[3];    
        float mVec4Val[4];    
        float mMat2x2Val[4];  
        float mMat4x4Val[16]; 
        GLuint mTextureVal;   
    };
};
