#include "scriptentitycore.hpp"
#include "../entity/entitymessages.hpp"
#include "../entity/entitysystem.hpp"

ScriptEntityCore::ScriptEntityCore(fea::MessageBus& bus, fea::EntityPtr entity, size_t id) :
    mBus(bus),
    mEntity(entity),
    mRefCount(0),
    mId(id)
{
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
    mBus.send<EntityMoveRequestedMessage>(EntityMoveRequestedMessage{mEntity->getId(), glm::vec3(x, y, z)});
}

void ScriptEntityCore::setPosition(const glm::vec3& vec)
{
    mBus.send<EntityMoveRequestedMessage>(EntityMoveRequestedMessage{mEntity->getId(), vec});
}

glm::vec3 ScriptEntityCore::getPosition()
{
    return mEntity->getAttribute<glm::vec3>("position");
}

bool ScriptEntityCore::isOnGround()
{
    return mEntity->getAttribute<bool>("on_ground");
}

void ScriptEntityCore::setId(size_t id)
{
    mId = id;
}
