#include "scriptinterface.h"
#include <iostream>

ScriptInterface::ScriptInterface(fea::MessageBus& bus, ScriptEngine& engine, ScriptModule& module, WorldInterface& worldInterface) : 
    mBus(bus),
    mEngine(engine), 
    mModule(module),
    mWorldInterface(worldInterface),
    onFrameCallback(engine),
    frameTick(0)
{
    mBus.addMessageSubscriber<FrameMessage>(*this);
}

ScriptInterface::~ScriptInterface()
{
    mBus.removeMessageSubscriber<FrameMessage>(*this);
}

void ScriptInterface::registerInterface()
{
    //printing
    int r = mEngine.getEngine()->RegisterGlobalFunction("void consolePrint(string text)", asMETHOD(ScriptInterface, scriptPrint), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);

    //entity
    r = mEngine.getEngine()->RegisterInterface("Entity"); assert(r >= 0);
    //mEngine.getEngine()->RegisterObjectType("EntityCore", sizeof(ScriptEntityCore), asOBJ_REF); assert(r >= 0);
    //r = mEngine.getEngine()->RegisterObjectBehaviour("EntityCore", asBEHAVE_FACTORY,"EntityCore@ spawnEntity()", asFUNCTIONPR(spawnEntityHere,(),ScriptEntityCore*), asCALL_STDCALL );
    //r = mEngine.getEngine()->RegisterObjectBehaviour("EntityCore", asBEHAVE_ADDREF, "void f()", asMETHOD(ScriptEntityCore,addRef), asCALL_THISCALL ); assert(r >= 0);
    //r = mEngine.getEngine()->RegisterObjectBehaviour("EntityCore", asBEHAVE_RELEASE, "void f()", asMETHOD(ScriptEntityCore,release), asCALL_THISCALL); assert(r >= 0);
    r = mEngine.getEngine()->RegisterGlobalFunction("Entity @createEntity(const string &in)", asMETHOD(ScriptInterface, createEntity), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);

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

void ScriptInterface::registerCallbacks()
{
    if(!mModule.hasErrors())
    {
        onFrameCallback.setFunction(mModule.getFunctionByDecl("void onFrame(int frameNumber)"));
    }
}

void ScriptInterface::handleMessage(const FrameMessage& received)
{
    if(!mModule.hasErrors())
    {
        //asIScriptContext* context = mEngine.getContext();

        onFrameCallback.execute(frameTick);
        frameTick++;
    }
}

void ScriptInterface::scriptPrint(std::string text)
{
    std::cout << "[script]: " << text;
}

void ScriptInterface::setGravity(float constant)
{
    mBus.sendMessage<GravityRequestedMessage>(GravityRequestedMessage(constant));
}

asIScriptObject* ScriptInterface::createEntity(const std::string& type)
{
    asIObjectType* objectType = mModule.getObjectTypeByDecl(type);
    
    asIScriptFunction *factory = objectType->GetFactoryByDecl(std::string(type + " @" + type +"()").c_str());


    if(factory)
    {
        asIScriptContext* ctx = mEngine.requestContext();
        // Prepare the context to call the factory function
        ctx->Prepare(factory);
        // Execute the call
        ctx->Execute();
        // Get the object that was created
        asIScriptObject *obj = *(asIScriptObject**)ctx->GetAddressOfReturnValue();
        // If you're going to store the object you must increase the reference,
        // otherwise it will be destroyed when the context is reused or destroyed.
        obj->AddRef();
        obj->AddRef();

        mWorldInterface.spawnEntityFromScriptHandle(type, glm::vec3(0.0f, 50.0f, 0.0f), obj);
        mEngine.freeContext(ctx);

        return obj;
    }
    else
    {
        return nullptr;
    }
}

asIScriptObject* ScriptInterface::instanciateScriptEntity(const std::string& type)
{
    asIObjectType* objectType = mModule.getObjectTypeByDecl(type);
    
    asIScriptFunction *factory = objectType->GetFactoryByDecl(std::string(type + " @" + type +"()").c_str());


    if(factory)
    {
        asIScriptContext* ctx = mEngine.requestContext();
        // Prepare the context to call the factory function
        ctx->Prepare(factory);
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
