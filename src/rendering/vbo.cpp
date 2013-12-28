#include "vbo.h"

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
            return sizeof(float) * 3;
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
            return 3;
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
    mVertexElements.push_back(e);
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
       // printf("bound element %s to %i. offset %i, stride: %i\n", v.attributeName.c_str(), id, v.offset, mCurrentSize);
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

VBO::VBO(){
    mMaxVertices = 0;
    mMaxIndices = 0;
    mCurrentVertex = 0;
    mCurrentIndex = 0;
    mAllocated = false;
    mVertexSize = 0;
    mCreatedBuffers = false;
}

void VBO::setMaxSize(unsigned int vertexCount, unsigned int indexCount){
    mMaxVertices = vertexCount;
    mMaxIndices = indexCount;
}

void VBO::allocateBuffers(){
    if(!mAllocated){
        mVertexSize = mVertexDeclaration.getVertexSize();
        mpVertexData = new char[mMaxVertices * mVertexSize + 1];//not sure why I have to add one extra char. let's say it's padding.
        mpIndexData  = new int[mMaxIndices];
        mAllocated = true;
    }
}

void VBO::deallocateBuffers(){
    if(mAllocated){
        delete[] mpVertexData;
        delete[] mpIndexData;
        mAllocated = false;
    }
}

VertexDeclaration& VBO::getVertexDeclaration(){
    return mVertexDeclaration;
}

unsigned int VBO::getIndexCount(){
    return mCurrentIndex; 
}
unsigned int VBO::getVertexCount(){
    return mCurrentVertex;
}

void VBO::createBuffers(){
    if(!mCreatedBuffers){
        glGenBuffers(2, mID);
        mCreatedBuffers = true;
    }
}

void VBO::destroyBuffers(){
    if(mCreatedBuffers){
        glDeleteBuffers(2, mID);
        mCreatedBuffers = false;
    }
}

void VBO::uploadVBO(){
	if(!mAllocated) return;
    createBuffers();
    bind();
    glBufferData(GL_ARRAY_BUFFER, mCurrentVertex * mVertexSize,(void*) mpVertexData, GL_STATIC_DRAW); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mCurrentIndex * sizeof(int), mpIndexData, GL_STATIC_DRAW);
    mDrawCount = mCurrentIndex;

    clear();
    unbind();
}
void VBO::reset(){
    mCurrentVertex = 0;
    mCurrentIndex = 0;
    mDrawCount = 0;
}

void VBO::clear(){
    deallocateBuffers(); 
    mCurrentVertex = 0;
    mCurrentIndex = 0;
}

void VBO::pushIndex(int i){
    if(mCurrentIndex < mMaxIndices){
        //printf("pushed index %u\n", i);
        mpIndexData[mCurrentIndex] = i;
        mCurrentIndex ++;
    }else printf("indices full\n");
}

char* VBO::getNextVertexPtr(int vertexAmount){
    if(mAllocated && mCurrentVertex + (vertexAmount - 1) < mMaxVertices){
        char* v = getVertexPtr(mCurrentVertex);
        mCurrentVertex += vertexAmount;
        return v;
    }
    return nullptr;
}

char* VBO::getVertexPtr(int i){
    if(mAllocated && i < mMaxVertices){
        char* v = mpVertexData;
        v += i * mVertexSize;
        return v;
    }
    return nullptr;
}

void VBO::bind(){
    glBindBuffer(GL_ARRAY_BUFFER, mID[VERTEX_BUFFER]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID[INDEX_BUFFER]);
}

void VBO::unbind(){
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void VBO::draw(){
    bind();
    mVertexDeclaration.bind();
    glDrawElements(mDrawType, mDrawCount, GL_UNSIGNED_INT, 0);
    mVertexDeclaration.unbind();
    unbind();
}

void VBO::draw(ShaderProgram& program){
    bind();
    mVertexDeclaration.bind(program);
    glDrawElements(mDrawType, mDrawCount, GL_UNSIGNED_INT, 0);
    mVertexDeclaration.unbind(program);
    unbind();
}

void VBO::setDrawType(GLenum type){
    mDrawType = type;
}

GLenum VBO::getDrawType(){
    return mDrawType;
}

