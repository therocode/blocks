#include "scriptinterface.h"
#include "scriptentitycore.h"
#include "asaddons/scriptmath.h"
#include "asaddons/scriptvectors.h"
#include <iostream>

ScriptInterface::ScriptInterface(fea::MessageBus& bus, ScriptEngine& engine, ScriptModule& module, WorldInterface& worldInterface, std::unordered_map<asIScriptObject*, size_t>& uglyReference) : 
    mBus(bus),
    mEngine(engine), 
    mModule(module),
    mWorldInterface(worldInterface),
    logName("script"),
    mUglyReference(uglyReference),
    onFrameCallback(engine),
    gameStartCallback(engine),
    frameTick(0)
{
    mBus.addMessageSubscriber<FrameMessage>(*this);
    mBus.addMessageSubscriber<GameStartMessage>(*this);

    ScriptEntityCore::sWorldInterface = &worldInterface;
    ScriptEntityCore::sBus = &bus;
}

ScriptInterface::~ScriptInterface()
{
    mBus.removeMessageSubscriber<FrameMessage>(*this);
    mBus.removeMessageSubscriber<GameStartMessage>(*this);
}

void ScriptInterface::registerInterface()
{
    //printing
    int r = mEngine.getEngine()->RegisterGlobalFunction("void consolePrint(string text)", asMETHOD(ScriptInterface, scriptPrint), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);

    //maths
    RegisterScriptMath(mEngine.getEngine());
    registerGlmVectors(mEngine.getEngine());

    //entity
    r = mEngine.getEngine()->RegisterInterface("IEntity"); assert(r >= 0);
    r = mEngine.getEngine()->RegisterGlobalFunction("IEntity@ createIEntity(const string &in, float x, float y, float z)", asMETHOD(ScriptInterface, createEntity), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
    r = mEngine.getEngine()->RegisterGlobalFunction("void removeEntity(IEntity@ entity)", asMETHOD(ScriptInterface, removeEntity), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);

    r = mEngine.getEngine()->RegisterObjectType("EntityCore", sizeof(ScriptEntityCore), asOBJ_REF); assert(r >= 0);
    r = mEngine.getEngine()->RegisterObjectBehaviour("EntityCore", asBEHAVE_ADDREF, "void f()", asMETHOD(ScriptEntityCore, addRef), asCALL_THISCALL ); assert(r >= 0);
    r = mEngine.getEngine()->RegisterObjectBehaviour("EntityCore", asBEHAVE_RELEASE, "void f()", asMETHOD(ScriptEntityCore, release), asCALL_THISCALL); assert(r >= 0);
    r = mEngine.getEngine()->RegisterObjectMethod("EntityCore", "void setPosition(float x, float y, float z)", asMETHODPR(ScriptEntityCore, setPosition, (float x, float y, float z), void), asCALL_THISCALL); assert(r >= 0);
    r = mEngine.getEngine()->RegisterObjectMethod("EntityCore", "void setPosition(const Vec3& in)", asMETHODPR(ScriptEntityCore, setPosition, (const glm::vec3&), void), asCALL_THISCALL); assert(r >= 0);
    r = mEngine.getEngine()->RegisterObjectMethod("EntityCore", "Vec3 getPosition()", asMETHOD(ScriptEntityCore, getPosition), asCALL_THISCALL); assert(r >= 0);

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
    r = mEngine.getEngine()->RegisterGlobalFunction("void setGravity(float constant)", asMETHOD(ScriptInterface, setGravity), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
}

void ScriptInterface::registerCallbacks(const std::map<size_t, ScriptEntity>& scriptEntities)
{
    if(!mModule.hasErrors())
    {
        onFrameCallback.setFunction(mModule.getFunctionByDecl("void onFrame(int frameNumber)"));
        gameStartCallback.setFunction(mModule.getFunctionByDecl("void gameStarted()"));
    }
}

void ScriptInterface::handleMessage(const FrameMessage& received)
{
    if(!mModule.hasErrors())
    {
        onFrameCallback.execute(frameTick);

        for(auto& object : mUglyReference)
        {
            ScriptMemberCallback<int32_t> callback(mEngine);
            asIScriptFunction* function = object.first->GetObjectType()->GetMethodByDecl("void onFrame(int frameNumber)");
            if(function)
            {
                callback.setFunction(function);
                callback.execute(object.first, frameTick);
            }
        }

        frameTick++;
    }
}

void ScriptInterface::handleMessage(const GameStartMessage& received)
{
    if(!mModule.hasErrors())
    {
        gameStartCallback.execute();
    }
}

void ScriptInterface::scriptPrint(std::string text)
{
    mBus.sendMessage<LogMessage>(LogMessage(text, logName, LogLevel::INFO));
}

void ScriptInterface::setGravity(float constant)
{
    mBus.sendMessage<GravityRequestedMessage>(GravityRequestedMessage(constant));
}

asIScriptObject* ScriptInterface::createEntity(const std::string& type, float x, float y, float z)
{
    asIObjectType* objectType = mModule.getObjectTypeByDecl(type);
    if(!objectType)
    {
        mBus.sendMessage<LogMessage>(LogMessage("Script runtime error: Tying to create entity of invalid type '" + type + "'", logName, LogLevel::ERROR));
        return nullptr;
    }
    
    asIScriptFunction *factory = objectType->GetFactoryByDecl(std::string(type + " @" + type +"(EntityCore@ core, uint id)").c_str());

    if(factory)
    {
        ScriptEntityCore* entityCore = new ScriptEntityCore(0);

        asIScriptContext* ctx = mEngine.requestContext();
        // Prepare the context to call the factory function
        ctx->Prepare(factory);
        // Add an entity core
        ctx->SetArgObject(0, entityCore);
        ctx->SetArgDWord(1, 0);
        // Execute the call
        ctx->Execute();
        // Get the object that was created
        asIScriptObject *obj = *(asIScriptObject**)ctx->GetAddressOfReturnValue();
        // If you're going to store the object you must increase the reference,
        // otherwise it will be destroyed when the context is reused or destroyed.
        obj->AddRef();
        obj->AddRef();

        size_t createdId = mWorldInterface.spawnEntityFromScriptHandle(type, glm::vec3(x, y, z), obj);
        entityCore->setId(createdId);

        asIScriptFunction* function = obj->GetObjectType()->GetMethodByDecl("void setId(uint id)");
        ctx->Prepare(function);
        ctx->SetObject(obj);
        ctx->SetArgDWord(0, createdId);
        ctx->Execute();

        mEngine.freeContext(ctx);

        return obj;
    }
    else
    {
        return nullptr;
    }
}

asIScriptObject* ScriptInterface::instanciateScriptEntity(const std::string& type, size_t id)
{
    asIObjectType* objectType = mModule.getObjectTypeByDecl(type);
    if(!objectType)
    {
        mBus.sendMessage<LogMessage>(LogMessage("Script runtime error: Trying to create entity of invalid type '" + type + "'", logName, LogLevel::ERROR));
        return nullptr;
    }
    
    asIScriptFunction* factory = objectType->GetFactoryByDecl(std::string(type + " @" + type +"(EntityCore@ core, uint id)").c_str());

    if(factory)
    {
        bool error = false;

        asIScriptContext* ctx = mEngine.requestContext();
        // Prepare the context to call the factory function
        ctx->Prepare(factory);

        // Add an entity core
        ctx->SetArgObject(0, new ScriptEntityCore(id));

        ctx->SetArgDWord(1, id);

        // Execute the call
        ctx->Execute();

        // Get the object that was created
        asIScriptObject *obj = *(asIScriptObject**)ctx->GetAddressOfReturnValue();
        // If you're going to store the object you must increase the reference,
        // otherwise it will be destroyed when the context is reused or destroyed.
        obj->AddRef();

        mEngine.freeContext(ctx);

        return obj;
    }
    else
    {
        return nullptr;
    }
}

void ScriptInterface::removeEntity(asIScriptObject* entity)
{
    mBus.sendMessage<RemoveScriptEntityMessage>(RemoveScriptEntityMessage(entity));
    entity->Release();
}
