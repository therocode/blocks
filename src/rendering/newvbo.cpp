#include "newvbo.h"

size_t VertexElement::getElementByteSize(unsigned int element){
    switch(element){
        case ELEMENT_FLOAT:
            return sizeof(float);
        case ELEMENT_FLOAT2:
            return sizeof(float) * 2;
        case ELEMENT_FLOAT3:
            return sizeof(float) * 3;
        case ELEMENT_FLOAT4:
            return sizeof(float) * 4;
        default:
            return sizeof(float);
    }
}

GLint VertexElement::getElementSize(unsigned int element){
    switch(element){
        case ELEMENT_FLOAT:
            return 1;
        case ELEMENT_FLOAT2:
            return 2;
        case ELEMENT_FLOAT3:
            return 3;
        case ELEMENT_FLOAT4:
            return 4;
        default:
            return 1;
    }
}

GLenum VertexElement::getElementType(unsigned int element){
    switch(element){
        case ELEMENT_FLOAT:
        case ELEMENT_FLOAT2:
        case ELEMENT_FLOAT3:
        case ELEMENT_FLOAT4:
            return GL_FLOAT;
        default:
            return GL_FLOAT;
    }
}

VertexDeclaration::VertexDeclaration(){

}

size_t VertexDeclaration::getVertexSize(){
    return mCurrentSize;
}

void VertexDeclaration::addElement(unsigned int elementType, unsigned int shaderLayout, std::string shaderAttribute){
    VertexElement e;
    e.elementType = elementType;
    e.layoutID = shaderLayout;
    e.attributeName = shaderAttribute;

    e.offset = mCurrentSize;
    mCurrentSize += VertexElement::getElementByteSize(elementType);

}

void VertexDeclaration::clear(){
    mCurrentSize = 0;
    mVertexElements.clear();
}

#define BUFFER_OFFSET(i) ((char*)NULL + (i))
void VertexDeclaration::bind(){
    for(VertexElement v : mVertexElements){
        glEnableVertexAttribArray(v.layoutID);
        glVertexAttribPointer(v.layoutID, 
                VertexElement::getElementSize(v.elementType), 
                VertexElement::getElementType(v.elementType), 
                GL_FALSE, mCurrentSize, BUFFER_OFFSET(v.offset));
    }
}

void VertexDeclaration::bind(ShaderProgram& program){
    for(VertexElement v : mVertexElements){
        GLint id = program.getAttribLocation(v.attributeName);
        glEnableVertexAttribArray(id);
        glVertexAttribPointer(id, 
                VertexElement::getElementSize(v.elementType), 
                VertexElement::getElementType(v.elementType), 
                GL_FALSE, mCurrentSize, BUFFER_OFFSET(v.offset));
    }
}

void VertexDeclaration::unbind(){
    for(VertexElement v : mVertexElements){
        glDisableVertexAttribArray(v.layoutID);
    }
}

void VertexDeclaration::unbind(ShaderProgram& program){
    for(VertexElement v : mVertexElements){
        glDisableVertexAttribArray(program.getAttribLocation(v.attributeName));
    }
}

//start VBO

NewVBO::NewVBO(){
    mMaxVertices = 0;
    mMaxIndices = 0;
    mAllocated = false;
    mVertexSize = 0;
}

void NewVBO::setMaxSize(unsigned int vertexCount, unsigned int indexCount){
    mMaxVertices = vertexCount;
    mMaxIndices = indexCount;
}

void NewVBO::allocateBuffers(){
    if(!mAllocated){
        mVertexSize = mVertexDeclaration.getVertexSize();
        mpVertexData = (char*)malloc(mMaxVertices * mVertexSize);
        mpIndexData  = new int[mMaxIndices];
        mAllocated = true;
    }
}

void NewVBO::deallocateBuffers(){
    if(mAllocated){
        free(mpVertexData);
        delete[] mpIndexData;
        mAllocated = false;
    }
}

VertexDeclaration& NewVBO::getVertexDeclaration(){
    return mVertexDeclaration;
}

void NewVBO::createBuffers(){
    if(!mCreatedBuffers){
        glGenBuffers(2, mID);
        mCreatedBuffers = true;
    }
}

void NewVBO::destroyBuffers(){
    if(mCreatedBuffers){
        glDeleteBuffers(2, mID);
        mCreatedBuffers = false;
    }
}

void NewVBO::uploadVBO(){
    createBuffers();
    bind();

    glBufferData(GL_ARRAY_BUFFER, mCurrentVertex, mpVertexData, GL_STATIC_DRAW); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mCurrentIndex, mpIndexData, GL_STATIC_DRAW);

    mDrawCount = mCurrentIndex;

    clear();
    unbind();
}

void NewVBO::clear(){
    deallocateBuffers(); 
    mCurrentVertex = 0;
    mCurrentIndex = 0;
}

void NewVBO::pushIndex(unsigned int i){
    if(mCurrentIndex < mMaxIndices){
        mpIndexData[mCurrentIndex] = i;
        mCurrentIndex ++;
    }
}

char* NewVBO::getNextVertexPtr(unsigned int vertexAmount){
    if(mAllocated && mCurrentVertex + (vertexAmount - 1) < mMaxVertices){
        char* v = getVertexPtr(mCurrentVertex);
        mCurrentVertex += vertexAmount;
        return v;
    }
    return nullptr;
}

char* NewVBO::getVertexPtr(unsigned int i){
    if(mAllocated && i < mMaxVertices){
        char* v = (char*)mpVertexData;
        v += i * mVertexSize;
        return v;
    }
    return nullptr;
}

void NewVBO::bind(){
    glBindBuffer(GL_ARRAY_BUFFER, mID[VERTEX_BUFFER]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID[INDEX_BUFFER]);
}

void NewVBO::unbind(){
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void NewVBO::draw(){
    bind();
    mVertexDeclaration.bind();
    glDrawElements(mDrawType, mDrawCount, GL_UNSIGNED_INT, 0);
    mVertexDeclaration.unbind();
    unbind();
}

void NewVBO::draw(ShaderProgram& program){
    bind();
    mVertexDeclaration.bind(program);
    glDrawElements(mDrawType, mDrawCount, GL_UNSIGNED_INT, 0);
    mVertexDeclaration.unbind(program);
    unbind();
}

void NewVBO::setDrawType(GLenum type){
    mDrawType = type;
}

GLenum NewVBO::getDrawType(){
    return mDrawType;
}

