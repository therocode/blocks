#include "scriptentity.h"

ScriptEntity::ScriptEntity(fea::WeakEntityPtr entity, asIScriptObject* scriptObject) : mEntity(entity), mScriptObject(scriptObject)
{
    mId = mEntity.lock()->getId();
}

ScriptEntity::~ScriptEntity()
{
    mScriptObject->Release();
}
