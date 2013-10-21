#include "scriptentitycore.h"
#include "../entity/entitymessages.h"

ScriptEntityCore::ScriptEntityCore(fea::MessageBus& bus, size_t id) : mBus(bus)
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
    mBus.sendMessage<EntityMoveRequestedMessage>(EntityMoveRequestedMessage(mId, glm::vec3(x, y, z)));
}

void ScriptEntityCore::setId(size_t id)
{
    mId = id;
}
