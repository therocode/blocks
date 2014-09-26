#include "entityinterface.hpp"
#include <angelscript.h>
#include <assert.h>
#include "../scriptentitycore.hpp"
#include "../../world/worldinterface.hpp"

EntityInterface::EntityInterface(fea::MessageBus& bus, WorldInterface& worldInterface, std::unordered_map<size_t, ScriptEntity>& scriptEntities) : 
    ScriptInterface(bus, worldInterface),
    mBus(bus),
    mScriptEntities(scriptEntities)
{
    subscribe(mBus, *this);
}

void EntityInterface::registerInterface(asIScriptEngine* engine)
{
    int32_t r = engine->RegisterInterface("IEntity"); assert(r >= 0);
    r = engine->RegisterGlobalFunction("IEntity@ createIEntity(const string &in, float x, float y, float z)", asMETHOD(EntityInterface, createEntity), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void removeEntity(uint id)", asMETHOD(EntityInterface, removeEntityFromId), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);

    r = engine->RegisterObjectType("EntityCore", sizeof(ScriptEntityCore), asOBJ_REF); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("EntityCore", asBEHAVE_ADDREF, "void f()", asMETHOD(ScriptEntityCore, addRef), asCALL_THISCALL ); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("EntityCore", asBEHAVE_RELEASE, "void f()", asMETHOD(ScriptEntityCore, release), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("EntityCore", "void setPosition(float x, float y, float z)", asMETHODPR(ScriptEntityCore, setPosition, (float x, float y, float z), void), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("EntityCore", "void setPosition(const Vec3& in)", asMETHODPR(ScriptEntityCore, setPosition, (const glm::vec3&), void), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("EntityCore", "Vec3 getPosition()", asMETHOD(ScriptEntityCore, getPosition), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("EntityCore", "bool isOnGround()", asMETHOD(ScriptEntityCore, isOnGround), asCALL_THISCALL); assert(r >= 0);
}

asIScriptObject* EntityInterface::createEntity(const std::string& type, float x, float y, float z)
{
    mBus.send(CreateEntityMessage{type, [&] (fea::EntityPtr e)
            {
                e->setAttribute("position", glm::vec3(x, y, z));
                e->setAttribute("current_world", 0u); //default world!?
            }});

    fea::WeakEntityPtr createdEntity;
    std::swap(createdEntity, mNewlyCreated);

    if(createdEntity.expired())
    {
        mBus.send(LogMessage{"Script runtime error: Id of created entity of type '" + type + "' is invalid", mLogName, LogLevel::ERR});
        return nullptr;
    }

    auto scriptEntity = mScriptEntities.find(createdEntity.lock()->getId());

    if(scriptEntity != mScriptEntities.end())
    {
        asIScriptObject* object = scriptEntity->second.getScriptObject();
        object->AddRef();
        return object;
    }
    else
    {
        return nullptr;
    }
}

void EntityInterface::removeEntityFromId(size_t id)
{
    mBus.send(RemoveEntityMessage{id});   
}

void EntityInterface::handleMessage(const EntityCreatedMessage& message)
{
    mNewlyCreated = message.entity;
}
