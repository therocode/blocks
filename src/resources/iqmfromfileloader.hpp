#pragma once
#include <string>
#include "rawmodel.hpp"
#include "iqm.h"
#include "resourceexception.hpp"

class IQMFromFileLoader
{
    public:
        RawModel load(const std::string& filename);
    private:
        char* readIqmHeader(char* headerPointer, iqmheader& result);
        char* readIqmVertexArray(char* vertexArrayPointer, iqmvertexarray& result);
        char* readIqmMesh(char* meshArrayPointer, iqmmesh& result);
        char* readIqmJoint(char* jointArrayPointer, iqmjoint& result);
        char* readIqmPose(char* poseArrayPointer, iqmpose& result);
        char* readIqmAnim(char* animArrayPointer, iqmanim& result);
};
