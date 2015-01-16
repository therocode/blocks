#include "scriptentitycore.hpp"
#include "../entity/entitymessages.hpp"
#include "../entity/entitysystem.hpp"

ScriptEntityCore::ScriptEntityCore(fea::MessageBus& bus, fea::EntityPtr entity, size_t id) :
    mBus(bus),
    mEntity(entity),
    mRefCount(0),
    mId(id)
{
    std::cout << "created script entity with id " << mId << "\n";
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

void ScriptEntityCore::setPosition(const glm::vec3& vec)
{
    mBus.send(EntityMoveRequestedMessage{mEntity->getId(), vec});
}

glm::vec3 ScriptEntityCore::getPosition()
{
    std::cout << "will try to copy \n";
    ScriptEntityCore copy = *this;
    std::cout << "copy's ID: " << copy.mId << "\n";
    std::cout << mId << " was the ID\n";
    return mEntity->getAttribute<glm::vec3>("position");
}

bool ScriptEntityCore::isOnGround()
{
    return mEntity->getAttribute<bool>("on_ground");
}

void ScriptEntityCore::setId(size_t id)
{
    std::cout << "setting entity id " << mId << " to " << id << "\n";
    mId = id;
}

WorldId ScriptEntityCore::getCurrentWorldId() const
{
    return mEntity->getAttribute<WorldId>("current_world");
}
