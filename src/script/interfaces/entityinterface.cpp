#include "entityinterface.hpp"
#include "../scriptstd.hpp"
#include <angelscript.h>
#include <assert.h>
#include "../../lognames.hpp"
#include "../scriptentitycore.hpp"
#include "../../gameinterface.hpp"

EntityInterface::EntityInterface(fea::MessageBus& bus, GameInterface& worldInterface, ScriptEntityMap& scriptEntities) : 
    ScriptInterface(bus, worldInterface),
    mScriptEntities(scriptEntities)
{
    subscribe(mBus, *this);
}

void EntityInterface::registerInterface(asIScriptEngine* engine)
{
    int32_t r = engine->RegisterInterface("IEntity"); assert(r >= 0);
    r = engine->RegisterGlobalFunction("IEntity@ createIEntity(const string &in, uint32 worldId, const Vec3& in)", asMETHOD(EntityInterface, createEntity), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
    r = engine->RegisterGlobalFunction("IEntity@ getIEntity(uint32 entityId)", asMETHOD(EntityInterface, getEntity), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void removeEntity(uint id)", asMETHOD(EntityInterface, removeEntityFromId), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);

    r = engine->RegisterObjectType("EntityCore", sizeof(ScriptEntityCore), asOBJ_REF); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("EntityCore", asBEHAVE_ADDREF, "void f()", asMETHOD(ScriptEntityCore, addRef), asCALL_THISCALL ); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("EntityCore", asBEHAVE_RELEASE, "void f()", asMETHOD(ScriptEntityCore, release), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("EntityCore", "void setPosition(const Vec3& in)", asMETHODPR(ScriptEntityCore, setPosition, (const glm::vec3&), void), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("EntityCore", "Vec3 getPosition()", asMETHOD(ScriptEntityCore, getPosition), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("EntityCore", "bool isOnGround()", asMETHOD(ScriptEntityCore, isOnGround), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("EntityCore", "uint32 getCurrentWorldId()", asMETHOD(ScriptEntityCore, getCurrentWorldId), asCALL_THISCALL); assert(r >= 0);
}

asIScriptObject* EntityInterface::createEntity(const std::string& type, WorldId worldId, const  glm::vec3 position)
{
    mBus.send(EntityRequestedMessage{type, [&] (fea::EntityPtr e)
            {
                e->setAttribute("position", position);
                e->setAttribute("current_world", worldId);
            }});

    fea::WeakEntityPtr createdEntity;
    std::swap(createdEntity, mNewlyCreated);

    if(createdEntity.expired())
    {
        mBus.send(LogMessage{"Script runtime error: Id of created entity of type '" + type + "' is invalid", gScriptName, LogLevel::ERR});
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

asIScriptObject* EntityInterface::getEntity(fea::EntityId id)
{
    auto iterator = mScriptEntities.find(id);

    if(iterator != mScriptEntities.end())
    {
        asIScriptObject* object = iterator->second.getScriptObject();
        object->AddRef();
        return object;
    }
    else
        return nullptr;
}

void EntityInterface::removeEntityFromId(fea::EntityId id)
{
    mBus.send(RemoveEntityRequestedMessage{id});   
}

void EntityInterface::handleMessage(const EntityCreatedMessage& message)
{
    mNewlyCreated = message.entity;
}
