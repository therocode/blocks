#include "scriptentity.h"

ScriptEntity::ScriptEntity(WeakEntityPtr entity) : mEntity(entity)
{
    mId = mEntity.lock()->getId();
}
