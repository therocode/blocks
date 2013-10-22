#include "scriptentitycore.h"
#include "../entity/entitymessages.h"


WorldInterface* ScriptEntityCore::sWorldInterface = nullptr;
fea::MessageBus* ScriptEntityCore::sBus = nullptr;

ScriptEntityCore::ScriptEntityCore(size_t id)
{
    mRefCount = 0;
    mId = id;
}

void ScriptEntityCore::addRef()
{
    mRefCount++;
}

void ScriptEntityCore::release()
{
    mRefCount--;
    if(mRefCount <= 0)
    {   
        delete this;
    }
}

void ScriptEntityCore::setPosition(float x, float y, float z)
{
    sBus->sendMessage<EntityMoveRequestedMessage>(EntityMoveRequestedMessage(mId, glm::vec3(x, y, z)));
}

void ScriptEntityCore::setId(size_t id)
{
    mId = id;
}