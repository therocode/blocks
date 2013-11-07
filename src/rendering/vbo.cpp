#include "vbo.h"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))
VBO::VBO():mCurrentVBOByteSize(0),mVBOCreated(false){
    mDrawType = GL_TRIANGLES;
    mVBOCreated = false;
    //Registering standard attributes.
    registerAttribute("vert",   0, VBOAttribute::ATTRIBUTE_FLOAT3);
    registerAttribute("normal", 1, VBOAttribute::ATTRIBUTE_FLOAT3);
    registerAttribute("color",  2, VBOAttribute::ATTRIBUTE_FLOAT3);
    registerAttribute("uv",     3, VBOAttribute::ATTRIBUTE_FLOAT2);
    //registerAttribute("bounds", 4, VBOAttribute::ATTRIBUTE_FLOAT4);
}

VBO::~VBO(){
}
void VBO::PushTriangle(const Triangle& t){
    for(Vertex i : t.vs){
        PushVertex(i);
    }
}
void VBO::PushVertex(const Vertex& v){
    mvVertices.push_back(v);	
    mvIndices.push_back(mvVertices.size() - 1);
}
void VBO::Clear(){
    for(auto t = mAttributes.begin(); t != mAttributes.end(); t++){
        t->second.clear();
    }
    mvVertices.clear();
    mvIndices.clear();
}

void VBO::PushRectangle(const Rectangle& r){
    int si = mvVertices.size() ;
    for(Vertex v : r.vs){
        mvVertices.push_back(v);
        AttribValue val;
        
        for(int i = 0; i <3; i++)
            val.floats[i] = v.normal[i];
        pushToAttribute("normal", val);
        
        for(int i = 0; i <3; i++)
            val.floats[i] = v.color[i];
        pushToAttribute("color", val);
        
        for(int i = 0; i <2; i++)
            val.floats[i] = v.uv[i];
        pushToAttribute("uv", val);

        for(int i = 0; i <3; i++)
            val.floats[i] = v.position[i];
        pushToAttribute("vert", val);
    }
    mvIndices.push_back(si);
    mvIndices.push_back(si + 1);
    mvIndices.push_back(si + 2);

    mvIndices.push_back(si + 2);
    mvIndices.push_back(si + 3);
    mvIndices.push_back(si);
}
void VBO::createDataArray(std::vector<float>& data){
    if(mAttributes.size() == 0) return;
    mStride = 0;
    for(auto t = mAttributes.begin(); t != mAttributes.end(); t++){
        mStride += t->second.getElementSize();
    }

    unsigned int elementCount = mAttributes.at("vert").getElementCount();

    for(unsigned int i = 0; i < elementCount; i ++){
        for(auto t = mAttributes.begin(); t != mAttributes.end(); t++){
            AttribValue v = t->second.getAttribElement(i);
            for(int val = 0; val < v.usedValues; val ++)
                data.push_back(v.floats[val]);
        }
    }   
}

void VBO::UpdateVBO(){
    std::vector<float> wow;
    CreateVBO();	
    createDataArray(wow);
    mCurrentVBOByteSize = sizeof(float) * wow.size();

    int idSize = sizeof(int) * mvIndices.size();
    BindBuffer();
    glBufferData(GL_ARRAY_BUFFER, mCurrentVBOByteSize, wow.data(), GL_STATIC_DRAW); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,idSize, mvIndices.data(), GL_STATIC_DRAW);
    UnbindBuffer();
    mDrawSize = idSize;
    Clear();
}
void VBO::DestroyVBO()
{
    Clear();
    if(mVBOCreated)
    {
        glDeleteBuffers(2, mID);
        mVBOCreated = false;
    }
}
void VBO::CreateVBO()
{
    if(!mVBOCreated){
        glGenBuffers(2, mID);
        mVBOCreated = true;
    }
}

VBOAttribute& VBO::getAttribute(std::string attribName){
    if(mAttributes.find(attribName) != mAttributes.end()){
        return mAttributes.at(attribName);
    }
}

VBOAttribute& VBO::getAttribute(GLuint attribID){

}

void VBO::registerAttribute(const std::string name, const int id, const int type ){
    VBOAttribute newAttrib(name, id, type);
    mAttributes.emplace(name, newAttrib);
}
void VBO::setMainAttribute(const std::string name){
    if(mAttributes.find(name) != mAttributes.end()){
        mMainAttrib = mAttributes.at(name).getAttributeID();
    }
}
void VBO::pushToAttribute(const std::string name, AttribValue v){
    if(mAttributes.find(name) != mAttributes.end()){
        if(mAttributes.at(name).getAttributeID() == mMainAttrib){
            for(auto t = mAttributes.begin(); t != mAttributes.end(); t++){
                if(t->second.getAttributeID() == mMainAttrib) continue;
                if(!t->second.mIsUpdated){
                    t->second.addElement(t->second.mLastValue);
                }
                t->second.mIsUpdated = false;
            }
        }
        mAttributes.at(name).addElement(v);
    }
}
void VBO::DrawVBO(){
    BindBuffer();
    int stride = sizeof(Vertex);
#if 0 
    //Use vertexpointer and stuff
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, stride, 0);
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3, GL_FLOAT, stride, BUFFER_OFFSET(3 * sizeof(float)));

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, stride, BUFFER_OFFSET(6 * sizeof(float)));

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, stride, BUFFER_OFFSET(9 * sizeof(float)));

    glDrawElements(mDrawType, mvIndices.size(), GL_UNSIGNED_INT, 0);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#else
    for(int i = 0; i< 4; i++){
        glEnableVertexAttribArray(i);
    }
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride , 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride , BUFFER_OFFSET(3 * sizeof(float)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride , BUFFER_OFFSET(6 * sizeof(float)));
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride , BUFFER_OFFSET(9 * sizeof(float)));
    glDrawElements(mDrawType, mvIndices.size(), GL_UNSIGNED_INT, 0);
    for(int i = 0; i< 4; i++){
        glDisableVertexAttribArray(i);
    }
/*    for(auto t = mAttributes.begin(); t != mAttributes.end(); t++){
        glEnableVertexAttribArray(t->second.getAttributeID());
        glVertexAttribPointer(t->second.getAttributeID(), t->second.getElementValueCount(), t->second.getType(), GL_FALSE, 
    }
*/
#endif
    UnbindBuffer();
}
void VBO::DrawVBO(ShaderProgram& program)
{
    if(!mVBOCreated)return;
    if(mDrawSize == 0) return;
    BindBuffer();
    /*int stride = sizeof(Vertex);

    GLint i = program.getAttribLocation("vert");
    glEnableVertexAttribArray(i);
    glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, stride , 0);

    i = program.getAttribLocation("normal");
    glEnableVertexAttribArray(i);
    glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, stride , BUFFER_OFFSET(6 * sizeof(float)));

    i = program.getAttribLocation("color");
    glEnableVertexAttribArray(i);
    glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, stride , BUFFER_OFFSET(3 * sizeof(float)));

    i = program.getAttribLocation("uv");
    glEnableVertexAttribArray(i);
    glVertexAttribPointer(i, 2, GL_FLOAT, GL_FALSE, stride , BUFFER_OFFSET(9 * sizeof(float)));

    glDrawElements(mDrawType, mDrawSize, GL_UNSIGNED_INT, 0);
*/
    unsigned int offset = 0;
    int idd = 0;

    for(auto t = mAttributes.begin(); t != mAttributes.end(); t++){
        idd++;
        GLint id = program.getAttribLocation(t->second.getName());
        glEnableVertexAttribArray(id);
        glVertexAttribPointer(id, t->second.getElementValueCount(), t->second.getType(), GL_FALSE, mStride, BUFFER_OFFSET(offset * sizeof(float)));
        offset += t->second.getElementValueCount();
    }

    glDrawElements(mDrawType, mDrawSize, GL_UNSIGNED_INT, 0);

    for(auto t = mAttributes.begin(); t != mAttributes.end(); t++)
        glDisableVertexAttribArray(program.getAttribLocation(t->second.getName()));

    UnbindBuffer();
}

void VBO::BindBuffer(){
    if(mVBOCreated){
        glBindBuffer(GL_ARRAY_BUFFER, mID[VERTICES]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID[INDICES]);
    }
}
void VBO::UnbindBuffer(){
    if(mVBOCreated){
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}
void VBO::DeleteBuffer(){
    if(mVBOCreated){
        glDeleteBuffers(2, mID);
        mVBOCreated = false;
    }
}
void VBO::SetDrawType(GLint type){
    mDrawType = type;
}
GLint VBO::GetDrawType(){
    return mDrawType;
}

int VBO::GetDrawAmount()
{
    return mDrawSize;
}
