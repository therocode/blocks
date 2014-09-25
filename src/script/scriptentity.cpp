#include "scriptentity.hpp"
#include <angelscript.h>

ScriptEntity::ScriptEntity(size_t id, fea::WeakEntityPtr entity, asIScriptObject* scriptObject) : 
    mId(id),
    mEntity(entity),
    mScriptObject(scriptObject)
{
}

ScriptEntity::ScriptEntity(const ScriptEntity& other) : mId(other.mId), mEntity(other.mEntity), mScriptObject(other.mScriptObject)
{
    mScriptObject->AddRef();
}

const ScriptEntity& ScriptEntity::operator=(const ScriptEntity& other)
{
    mId = other.mId;
    mEntity = other.mEntity;
    mScriptObject = other.mScriptObject;
    mScriptObject->AddRef();
    return *this;
}

ScriptEntity::~ScriptEntity()
{
    mScriptObject->Release();
}

asIScriptObject* ScriptEntity::getScriptObject()
{
    return mScriptObject;
}
