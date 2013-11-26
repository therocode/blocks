#include "scripthandler.h"
#include "scriptmessages.h"
#include "asaddons/scriptmath.h"
#include "asaddons/scriptvectors.h"
#include "../entity/entitymessages.h"
#include "../utilities/folderexploder.h"
#include "scriptentitycore.h"
#include <iostream>
#include "world/worldmessages.h"

ScriptHandler::ScriptHandler(fea::MessageBus& bus, WorldInterface& worldInterface) : 
    mEngine(bus),
    mBus(bus),
    mScripts(mEngine.createModule("scripts")),
    mWorldInterface(worldInterface),
    logName("script"),
    onFrameCallback(mEngine),
    gameStartCallback(mEngine),
    mEntityCreator([] (const std::string& type, const glm::vec3& position) { return fea::EntityPtr(nullptr); }),
    frameTick(0)
{
    mBus.addMessageSubscriber<RebuildScriptsRequestedMessage>(*this);
    mBus.addMessageSubscriber<EntityCreatedMessage>(*this);
    mBus.addMessageSubscriber<EntityRemovedMessage>(*this);
    mBus.addMessageSubscriber<ScriptEntityFinishedMessage>(*this);

    mBus.addMessageSubscriber<FrameMessage>(*this);
    mBus.addMessageSubscriber<GameStartMessage>(*this);
    mBus.addMessageSubscriber<EntityOnGroundMessage>(*this);

    ScriptEntityCore::sWorldInterface = &worldInterface;
    ScriptEntityCore::sBus = &bus;
    setEntityCreator(mWorldInterface.getEntityCreator());
}

ScriptHandler::~ScriptHandler()
{
    mBus.removeMessageSubscriber<RebuildScriptsRequestedMessage>(*this);
    mBus.removeMessageSubscriber<EntityCreatedMessage>(*this);
    mBus.removeMessageSubscriber<EntityRemovedMessage>(*this);
    mBus.removeMessageSubscriber<ScriptEntityFinishedMessage>(*this);

    mBus.removeMessageSubscriber<FrameMessage>(*this);
    mBus.removeMessageSubscriber<GameStartMessage>(*this);
    mBus.removeMessageSubscriber<EntityOnGroundMessage>(*this);
}

void ScriptHandler::setup()
{
    mEngine.setup();
    registerInterface();

    FolderExploder exploder;

    sourceFiles.clear();

    exploder.explodeFolder("data", ".*\\.as", sourceFiles);
    for(auto& string : sourceFiles)
    {
        mBus.sendMessage<LogMessage>(LogMessage("Adding " + string + " for compilation.", logName, LogLevel::VERB));
    }

    mBus.sendMessage<LogMessage>(LogMessage("Compiling scripts...", logName, LogLevel::INFO));
    bool succeeded = mScripts.compileFromSourceList(sourceFiles);
    mBus.sendMessage<LogMessage>(LogMessage("Compilation process over.", logName, LogLevel::INFO));

    if(succeeded)
    {
        mBus.sendMessage<LogMessage>(LogMessage("Setting up script callbacks...", logName, LogLevel::VERB));
        registerCallbacks(scriptEntities);
        mBus.sendMessage<LogMessage>(LogMessage("Compilation process over.", logName, LogLevel::VERB));
        mBus.sendMessage<LogMessage>(LogMessage("Done setting up callbacks.", logName, LogLevel::VERB));
    }
}

void ScriptHandler::destroy()
{
    scriptEntities.clear();
    mEngine.destroyModule(mScripts);
    mEngine.destroy();
}

void ScriptHandler::handleMessage(const RebuildScriptsRequestedMessage& message)
{
    mBus.sendMessage<LogMessage>(LogMessage("Compiling scripts...", logName, LogLevel::INFO));
    bool succeeded = mScripts.compileFromSourceList(sourceFiles);
    mBus.sendMessage<LogMessage>(LogMessage("Compilation process over.", logName, LogLevel::INFO));

    if(succeeded)
    {
        mBus.sendMessage<LogMessage>(LogMessage("Setting up script callbacks...", logName, LogLevel::VERB));
        registerCallbacks(scriptEntities);
        mBus.sendMessage<LogMessage>(LogMessage("Compilation process over.", logName, LogLevel::VERB));
        mBus.sendMessage<LogMessage>(LogMessage("Done setting up callbacks.", logName, LogLevel::VERB));
    }
}

void ScriptHandler::handleMessage(const EntityCreatedMessage& message)
{
}

void ScriptHandler::handleMessage(const EntityRemovedMessage& message)
{
    size_t id;
    std::tie(id) = message.data;

    scriptEntities.erase(id);
}

void ScriptHandler::handleMessage(const ScriptEntityFinishedMessage& message)
{
    size_t id;
    asIScriptObject* obj;
    fea::WeakEntityPtr entity;

    std::tie(id, obj, entity) = message.data;

    ScriptEntity scriptEntity(id, entity, obj);

    scriptEntities.emplace(id, scriptEntity);
}

void ScriptHandler::handleMessage(const FrameMessage& received)
{
    if(!mScripts.hasErrors())
    {
        onFrameCallback.execute(frameTick);

        for(auto& entity : scriptEntities)
        {
            ScriptMemberCallback<int32_t> callback(mEngine);
            asIScriptObject* object = entity.second.getScriptObject();
            asIScriptFunction* function = object->GetObjectType()->GetMethodByDecl("void onFrame(int frameNumber)");
            if(function)
            {
                callback.setFunction(function);
                callback.execute(object, frameTick);
            }
        }

        frameTick++;
    }
}

void ScriptHandler::handleMessage(const GameStartMessage& received)
{
    if(!mScripts.hasErrors())
    {
        gameStartCallback.execute();
    }
}

void ScriptHandler::handleMessage(const EntityOnGroundMessage& received)
{
    bool landed;
    size_t id;

    std::tie(id, landed) = received.data;

    if(!mScripts.hasErrors())
    {
        for(auto& entity : scriptEntities)
        {
            if(id == entity.first)
            {
                ScriptMemberCallback<bool> callback(mEngine);
                asIScriptObject* object = entity.second.getScriptObject();
                asIScriptFunction* function = object->GetObjectType()->GetMethodByDecl("void onGround(bool landed)");
                if(function)
                {
                    callback.setFunction(function);
                    callback.execute(object, landed);
                }
            }
        }
    }
}

void ScriptHandler::registerInterface()
{
    //printing
    int r = mEngine.getEngine()->RegisterGlobalFunction("void consolePrint(string text)", asMETHODPR(ScriptHandler, scriptPrint, (const std::string&), void), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
    r = mEngine.getEngine()->RegisterGlobalFunction("void consolePrint(string text, uint level)", asMETHODPR(ScriptHandler, scriptPrint, (const std::string&, uint32_t), void), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);

    //maths
    RegisterScriptMath(mEngine.getEngine());
    registerGlmVectors(mEngine.getEngine());

    //entity
    r = mEngine.getEngine()->RegisterInterface("IEntity"); assert(r >= 0);
    r = mEngine.getEngine()->RegisterGlobalFunction("IEntity@ createIEntity(const string &in, float x, float y, float z)", asMETHOD(ScriptHandler, createEntity), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
    r = mEngine.getEngine()->RegisterGlobalFunction("void removeEntity(IEntity@ entity)", asMETHOD(ScriptHandler, removeEntity), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);

    r = mEngine.getEngine()->RegisterObjectType("EntityCore", sizeof(ScriptEntityCore), asOBJ_REF); assert(r >= 0);
    r = mEngine.getEngine()->RegisterObjectBehaviour("EntityCore", asBEHAVE_ADDREF, "void f()", asMETHOD(ScriptEntityCore, addRef), asCALL_THISCALL ); assert(r >= 0);
    r = mEngine.getEngine()->RegisterObjectBehaviour("EntityCore", asBEHAVE_RELEASE, "void f()", asMETHOD(ScriptEntityCore, release), asCALL_THISCALL); assert(r >= 0);
    r = mEngine.getEngine()->RegisterObjectMethod("EntityCore", "void setPosition(float x, float y, float z)", asMETHODPR(ScriptEntityCore, setPosition, (float x, float y, float z), void), asCALL_THISCALL); assert(r >= 0);
    r = mEngine.getEngine()->RegisterObjectMethod("EntityCore", "void setPosition(const Vec3& in)", asMETHODPR(ScriptEntityCore, setPosition, (const glm::vec3&), void), asCALL_THISCALL); assert(r >= 0);
    r = mEngine.getEngine()->RegisterObjectMethod("EntityCore", "Vec3 getPosition()", asMETHOD(ScriptEntityCore, getPosition), asCALL_THISCALL); assert(r >= 0);
    r = mEngine.getEngine()->RegisterObjectMethod("EntityCore", "bool isOnGround()", asMETHOD(ScriptEntityCore, isOnGround), asCALL_THISCALL); assert(r >= 0);

    //string conversion
    r = mEngine.getEngine()->RegisterGlobalFunction("string toString(int num)", asFUNCTIONPR(std::to_string, (int32_t), std::string), asCALL_CDECL); assert(r >= 0);
    r = mEngine.getEngine()->RegisterGlobalFunction("string toString(uint num)", asFUNCTIONPR(std::to_string, (uint32_t), std::string), asCALL_CDECL); assert(r >= 0);
    r = mEngine.getEngine()->RegisterGlobalFunction("string toString(int16 num)", asFUNCTIONPR(std::to_string, (int32_t), std::string), asCALL_CDECL); assert(r >= 0);
    r = mEngine.getEngine()->RegisterGlobalFunction("string toString(uint16 num)", asFUNCTIONPR(std::to_string, (uint32_t), std::string), asCALL_CDECL); assert(r >= 0);
    r = mEngine.getEngine()->RegisterGlobalFunction("string toString(int64 num)", asFUNCTIONPR(std::to_string, (int64_t), std::string), asCALL_CDECL); assert(r >= 0);
    r = mEngine.getEngine()->RegisterGlobalFunction("string toString(uint64 num)", asFUNCTIONPR(std::to_string, (uint64_t), std::string), asCALL_CDECL); assert(r >= 0);
    r = mEngine.getEngine()->RegisterGlobalFunction("string toString(float num)", asFUNCTIONPR(std::to_string, (float), std::string), asCALL_CDECL); assert(r >= 0);
    r = mEngine.getEngine()->RegisterGlobalFunction("string toString(double num)", asFUNCTIONPR(std::to_string, (double), std::string), asCALL_CDECL); assert(r >= 0);

    //random
    r = mEngine.getEngine()->RegisterGlobalFunction("uint randomInt()", asMETHOD(Random, randomInt), asCALL_THISCALL_ASGLOBAL, &random); assert(r >= 0);
    r = mEngine.getEngine()->RegisterGlobalFunction("int randomIntRange(int start, int end)", asMETHOD(Random, randomIntRange), asCALL_THISCALL_ASGLOBAL, &random); assert(r >= 0);
    r = mEngine.getEngine()->RegisterGlobalFunction("float randomFloatRange(float start, float end)", asMETHOD(Random, randomFloatRange), asCALL_THISCALL_ASGLOBAL, &random); assert(r >= 0);
    r = mEngine.getEngine()->RegisterGlobalFunction("bool randomChance(float chance)", asMETHOD(Random, randomChance), asCALL_THISCALL_ASGLOBAL, &random); assert(r >= 0);

    //physics
    r = mEngine.getEngine()->RegisterGlobalFunction("void setGravity(float constant)", asMETHOD(ScriptHandler, setGravity), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
    r = mEngine.getEngine()->RegisterGlobalFunction("void applyImpulseOnEntity(uint id, const Vec3& in)", asMETHOD(ScriptHandler, applyImpulse), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);

    //landscape
    r = mEngine.getEngine()->RegisterGlobalFunction("void setVoxelType(float x, float y, float z, uint16 type)", asMETHODPR(ScriptHandler, setVoxelType, (float x, float y, float z, uint16_t type), void), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
    r = mEngine.getEngine()->RegisterGlobalFunction("void setVoxelType(const Vec3& in, uint16 type)", asMETHODPR(ScriptHandler, setVoxelType, (const glm::vec3&, uint16_t type), void), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
    r = mEngine.getEngine()->RegisterGlobalFunction("uint16 getVoxelType(float x, float y, float z)", asMETHODPR(ScriptHandler, getVoxelType, (float x, float y, float z), VoxelType), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
    r = mEngine.getEngine()->RegisterGlobalFunction("uint16 getVoxelType(const Vec3& in)", asMETHODPR(ScriptHandler, getVoxelType, (const glm::vec3&), VoxelType), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
}

void ScriptHandler::registerCallbacks(const std::map<size_t, ScriptEntity>& scriptEntities)
{
    if(!mScripts.hasErrors())
    {
        onFrameCallback.setFunction(mScripts.getFunctionByDecl("void onFrame(int frameNumber)"));
        gameStartCallback.setFunction(mScripts.getFunctionByDecl("void gameStarted()"));
    }
}

void ScriptHandler::scriptPrint(const std::string& text)
{
    mBus.sendMessage<LogMessage>(LogMessage(text, logName, LogLevel::INFO));
}

void ScriptHandler::scriptPrint(const std::string& text, uint32_t level)
{
    mBus.sendMessage<LogMessage>(LogMessage(text, logName, level));
}

void ScriptHandler::setGravity(float constant)
{
    mBus.sendMessage<GravityRequestedMessage>(GravityRequestedMessage(constant));
}

asIScriptObject* ScriptHandler::createEntity(const std::string& type, float x, float y, float z)
{
    fea::WeakEntityPtr createdEntity = mEntityCreator(type, glm::vec3(x, y, z));

    if(createdEntity.expired())
    {
        mBus.sendMessage<LogMessage>(LogMessage("Script runtime error: Id of created entity of type '" + type + "' is invalid. Abandoning script initialisation", logName, LogLevel::ERR));
        return nullptr;
    }

    asIObjectType* objectType = mScripts.getObjectTypeByDecl(type);
    if(!objectType)
    {
        mBus.sendMessage<LogMessage>(LogMessage("Script runtime error: Trying to create entity of invalid type '" + type + "'", logName, LogLevel::ERR));
        return nullptr;
    }

    asIScriptFunction *factory = objectType->GetFactoryByDecl(std::string(type + " @" + type +"(EntityCore@ core, uint id)").c_str());

    if(factory)
    {
        fea::EntityId id = createdEntity.lock()->getId();

        ScriptEntityCore* entityCore = new ScriptEntityCore(id);

        asIScriptContext* ctx = mEngine.requestContext();
        // Prepare the context to call the factory function
        ctx->Prepare(factory);
        // Add an entity core
        ctx->SetArgObject(0, entityCore);
        ctx->SetArgDWord(1, id);
        // Execute the call
        ctx->Execute();
        // Get the object that was created
        asIScriptObject *obj = *(asIScriptObject**)ctx->GetAddressOfReturnValue();
        // If you're going to store the object you must increase the reference,
        // otherwise it will be destroyed when the context is reused or destroyed.
        obj->AddRef();
        obj->AddRef();

        mBus.sendMessage<LogMessage>(LogMessage("Created entity id " + std::to_string(id) + " of type '" + type + "'", logName, LogLevel::VERB));

        mEngine.freeContext(ctx);

        ScriptEntity scriptEntity(id, createdEntity, obj);
        scriptEntities.emplace(id, scriptEntity);

        return obj;
    }
    else
    {
        mBus.sendMessage<LogMessage>(LogMessage("Script runtime error: Entity of type '" + type + "' has no valid factory function", logName, LogLevel::ERR));
        return nullptr;
    }
    return nullptr;
}

void ScriptHandler::removeEntity(asIScriptObject* entity)
{
    //tbi
}

void ScriptHandler::applyImpulse(size_t id, const glm::vec3& force)
{
    mBus.sendMessage<PhysicsImpulseMessage>(PhysicsImpulseMessage(id, force));
}

void ScriptHandler::setVoxelType(float x, float y, float z, uint16_t type)
{
    mBus.sendMessage<SetVoxelMessage>(SetVoxelMessage(VoxelWorldCoordinate(floor(x), floor(y), floor(z)), type));
}

void ScriptHandler::setVoxelType(const glm::vec3& coordinate, uint16_t type)
{
    setVoxelType(coordinate.x, coordinate.y, coordinate.z, type);
}

VoxelType ScriptHandler::getVoxelType(float x, float y, float z)
{
    return mWorldInterface.getVoxelType(x, y, z);
}

VoxelType ScriptHandler::getVoxelType(const glm::vec3& coordinate)
{
    return mWorldInterface.getVoxelType(coordinate);
}

void ScriptHandler::setEntityCreator(EntityCreator creator)
{
    mEntityCreator = creator;
}
