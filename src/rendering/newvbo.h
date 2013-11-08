#pragma once
#include <vector>
#include <featherkit/rendering/opengl.h>
#include "blockstd.h"
#include "shaderprogram.h"
#include <map>

struct VertexElement{
    enum ELEMENT_TYPES{
        ELEMENT_FLOAT = 0,
        ELEMENT_FLOAT2,
        ELEMENT_FLOAT3,
        ELEMENT_FLOAT4
    };

    static size_t getElementByteSize(unsigned int element);

    static GLint  getElementSize(unsigned int element);
    static GLenum getElementType(unsigned int element);

    unsigned int elementType;
    unsigned int layoutID;
    std::string  attributeName;

    unsigned int offset = 0;
};

class VertexDeclaration{
    public:
        VertexDeclaration();
        size_t getVertexSize();
        void   addElement(unsigned int elementType, unsigned int shaderLayout, std::string shaderAttribute);
        void   clear();
        void   bind();
        void   bind(ShaderProgram& program);
        void   unbind();
        void   unbind(ShaderProgram& program);
    private:
        size_t mCurrentSize = 0;
        std::vector<VertexElement> mVertexElements;
};

enum VBOType{
    VERTEX_BUFFER = 0, 
    INDEX_BUFFER
};

class VBO{
    protected:
        bool  mAllocated;
        char* mpVertexData;
        int * mpIndexData;
        unsigned int mCurrentVertex;
        unsigned int mCurrentIndex;


        size_t mVertexSize;
        unsigned int mMaxVertices;
        unsigned int mMaxIndices;
        VertexDeclaration mVertexDeclaration;

        GLenum mDrawType = GL_TRIANGLES;
        GLsizei mDrawCount;

        GLuint mID[2] = {0,0};
        bool mCreatedBuffers;

    public:
		char* getRawData(){return mpVertexData;}
        size_t getElementSize(){return mVertexDeclaration.getVertexSize();}
        VBO();

        VertexDeclaration& getVertexDeclaration();
        unsigned int getIndexCount();
        unsigned int getVertexCount();
        void setMaxSize(unsigned int vertexCount, unsigned int indexCount);

        void allocateBuffers();
        void deallocateBuffers();

        void reset();

        ///uploads to gpu. will clear the local stuff.
        void uploadVBO();

        void clear();
		
        void  pushIndex(int i);

        //Gets pointer to new vertex that can be modified.
        char* getNextVertexPtr(int vertexAmount = 1);


        //Gets pointer to vertex at this position.
        char* getVertexPtr(int i);

        void createBuffers();
        void destroyBuffers();

        void bind();
        void unbind();

        void draw();
        void draw(ShaderProgram& program);

        void setDrawType(GLenum type = GL_TRIANGLES);
        GLenum getDrawType();
};
