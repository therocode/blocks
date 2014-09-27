#include "scriptentitycore.hpp"
#include "../entity/entitymessages.hpp"
#include "../gameinterface.hpp"

GameInterface* ScriptEntityCore::sGameInterface = nullptr;
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
    sBus->send<EntityMoveRequestedMessage>(EntityMoveRequestedMessage{(fea::EntityId)mId, glm::vec3(x, y, z)});
}

void ScriptEntityCore::setPosition(const glm::vec3& vec)
{
    sBus->send<EntityMoveRequestedMessage>(EntityMoveRequestedMessage{(fea::EntityId)mId, vec});
}

glm::vec3 ScriptEntityCore::getPosition()
{
    return sGameInterface->findEntity(mId).lock()->getAttribute<glm::vec3>("position");
}

bool ScriptEntityCore::isOnGround()
{
    return sGameInterface->findEntity(mId).lock()->getAttribute<bool>("on_ground");
}

void ScriptEntityCore::setId(size_t id)
{
    mId = id;
}
