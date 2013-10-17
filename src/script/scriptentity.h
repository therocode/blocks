#pragma once
#include <featherkit/entitysystem.h>

class ScriptEntity
{
    public:
        ScriptEntity(fea::WeakEntityPtr entity);
    private:
        fea::EntityId mId;
        fea::WeakEntityPtr mEntity;
};
