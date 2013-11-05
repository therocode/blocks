#pragma once
#include <vector>
#include <featherkit/rendering/opengl.h>
#include "../blockstd.h"
#include "shaderprogram.h"
#include <map>

struct Vertex{
    float position[3] = {1,1,1};
    float color[3] = {1,1,1};
    float normal[3] = {1,1,1};
    float uv[2] = {1,1};
};
struct Triangle{
    Vertex vs[3];
};
struct Rectangle{
    Vertex vs[4];
    void setPosition(int i, float x, float y, float z){
        vs[i].position[0] = x;
        vs[i].position[1] = y;
        vs[i].position[2] = z;
    }
    void setAxisPosition(int axis, float x)
    {
        vs[0].position[axis] = x;
        vs[1].position[axis] = x;
        vs[2].position[axis] = x;
    }
    void setUV(int i, float u, float v){
        vs[i].uv[0] = u;
        vs[i].uv[1] = v;
    }
    void calculateNormal(){
        float* tPos = vs[0].position;
        glm::vec3 v0(tPos[0], tPos[1], tPos[2]);		

        tPos = vs[1].position;
        glm::vec3 v1(tPos[0], tPos[1], tPos[2]);

        tPos = vs[3].position;
        glm::vec3 v2(tPos[0], tPos[1], tPos[2]);

        v2 = v2 - v0;
        v1 = glm::cross(v2, v0 - v1);

        for(int i = 0; i < 4; i++){
            vs[i].normal[0] = v1.x;
            vs[i].normal[1] = v1.y;
            vs[i].normal[2] = v1.z;
        }	
    }
    void setColor(float r, float g, float b){
        for(Vertex &v : vs){
            v.color[0] = r;
            v.color[1] = g;
            v.color[2] = b;
        }	
    };
};

struct AttribValue{
    union{
        float floats[4];
        int  ints[4];
        char chars[4];
    };
    float& floatAtIndex(int i){
        return floats[i];
    }
    int&  intAtIndex(int i){
        return ints[i];
    }
    AttribValue(){
        for(int i = 0; i < 4; i ++)
            ints[i] = 0;
    }
    AttribValue(int x, int y = 0, int z = 0, int w = 0){
        ints[0] = x;
        ints[1] = y;
        ints[2] = z;
        ints[3] = w;
    }
    AttribValue(float x, float y = 0.f, float z = 0.f, float w = 0.f){
        floats[0] = x;
        floats[1] = y;
        floats[2] = z;
        floats[3] = w;
    }
};
class VBOAttribute{
    public:
        enum ATTRIBUTE_TYPES{
            ATTRIBUTE_FLOAT = 0,
            ATTRIBUTE_FLOAT2,
            ATTRIBUTE_FLOAT3,
            ATTRIBUTE_FLOAT4,
            ATTRIBUTE_INT,
            ATTRIBUTE_INT2,
            ATTRIBUTE_INT3,
            ATTRIBUTE_INT4
        };
    protected:
        std::vector<AttribValue> mData;
        GLenum mType = GL_FLOAT;
        GLuint mLayoutID = 0;
        int    mLayoutType;
        std::string mLayoutName = "";
        ///Size of one value in one element
        size_t mValueSize;
        ///Amount of values in one element.
        int    mElementSize;
        int    mAttributeType;

    public:
        VBOAttribute(std::string layoutName, GLuint layoutID, int attributeType){
            mLayoutName = layoutName;
            mLayoutID = layoutID;
            mAttributeType = attributeType;
            switch(mAttributeType){
                case ATTRIBUTE_FLOAT:
                case ATTRIBUTE_INT:
                    mElementSize = 1;
                    break;
                case ATTRIBUTE_FLOAT2:
                case ATTRIBUTE_INT2:
                    mElementSize = 2;
                    break;
                case ATTRIBUTE_FLOAT3:
                case ATTRIBUTE_INT3:
                    mElementSize = 3;
                    break;
                case ATTRIBUTE_FLOAT4:
                case ATTRIBUTE_INT4:
                    mElementSize = 4;
                    break;
                default:
                    mElementSize = 0;
            }
        }

        void addElement(AttribValue values){
            mData.push_back(values);
        }

        AttribValue getAttribElement(int index){
            if(index < mData.size())
                return mData[index];
            return AttribValue();
        }

        GLenum getType()    const {
            return mType;    
        }
        GLuint getLayoutID()   const {
            return mLayoutID;   
        }
};

enum VBOIDs{
    VERTICES = 0, 
    INDICES
};

class VBO {
    public:
        VBO();
        ~VBO();
        void PushTriangle(const Triangle& t);
        void PushRectangle(const Rectangle& r);
        void PushVertex(const Vertex& v);
        void Clear();
        void UpdateVBO();
        void DrawVBO();
        void DrawVBO(ShaderProgram& program);
        void SetDrawType(GLint type = GL_TRIANGLES);
        GLint GetDrawType();
        void DestroyVBO();
        void CreateVBO();
        int	GetID(int which) const {if(which == VERTICES) return mID[0]; else return mID[1];}
        int GetDrawAmount();
        VBOAttribute& getAttribute(std::string attribName);
        VBOAttribute& getAttribute(GLuint attribID);
        void registerAttribute(const std::string name, int id, int type);
        void setMainAttribute(const std::string name);
        void pushToAttribute(const std::string name, AttribValue v);
    private:
        std::map<std::string, VBOAttribute> mAttributes;
        GLuint mID[2];
        int mCurrentVBOByteSize;
        bool mVBOCreated;
        std::vector<Vertex> mvVertices;
        std::vector<int>	mvIndices;
        void BindBuffer();
        void UnbindBuffer();
        void DeleteBuffer();
        GLint mDrawType;
        GLuint mDrawSize;
};
