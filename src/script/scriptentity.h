#pragma once
#include <featherkit/entitysystem.h>
#include <angelscript.h>

class ScriptEntity
{
    public:
        ScriptEntity(fea::WeakEntityPtr entity, asIScriptObject* scriptObject);
    private:
        fea::EntityId mId;
        fea::WeakEntityPtr mEntity;
        asIScriptObject* mScriptObject;
};
