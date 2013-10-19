#pragma once
#include <featherkit/entitysystem.h>
#include <angelscript.h>

class ScriptEntity
{
    public:
        ScriptEntity(size_t id, fea::WeakEntityPtr entity, asIScriptObject* scriptObject);
        ScriptEntity(const ScriptEntity& other);
        const ScriptEntity& operator=(const ScriptEntity& other);
        ~ScriptEntity();
    private:
        fea::EntityId mId;
        fea::WeakEntityPtr mEntity;
        asIScriptObject* mScriptObject;
};
