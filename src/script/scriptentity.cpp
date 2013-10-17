#include "scriptentity.h"

ScriptEntity::ScriptEntity(fea::WeakEntityPtr entity) : mEntity(entity)
{
    mId = mEntity.lock()->getId();
}
