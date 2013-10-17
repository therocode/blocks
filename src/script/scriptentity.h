#pragma once

class ScriptEntity
{
    public:
        ScriptEntity(WeakEntityPtr entity);
    private:
        EntityId mId;
        WeakEntityPtr mEntity;
};
