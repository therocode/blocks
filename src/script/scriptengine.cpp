#include "scriptengine.h"
#include <iostream>
#include <assert.h>
#include "asaddons/scriptstdstring.h"
#include "asaddons/scriptarray.h"
#include "../application/applicationmessages.h"

ScriptEngine::ScriptEngine(fea::MessageBus& bus) : 
    mBus(bus),
    mContextsInUse(0)
{
};

ScriptEngine::~ScriptEngine()
{
}

void ScriptEngine::setup()
{
    mContextsInUse = 0;

    mEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

    RegisterScriptArray(mEngine, true);
    RegisterStdString(mEngine);
    RegisterStdStringUtils(mEngine);

    int32_t r = mEngine->SetMessageCallback(asMETHOD(ScriptEngine, messageCallback), this, asCALL_THISCALL); assert( r >= 0 );
}

void ScriptEngine::destroy()
{
    for(auto context : mContexts)
    {
        context->Release();
    }
    mEngine->Release();
}

ScriptModule ScriptEngine::createModule(const std::string& name)
{
    return ScriptModule(name, *this);
}

void ScriptEngine::destroyModule(ScriptModule& module)
{
     mEngine->DiscardModule(module.getName().c_str());
}

void ScriptEngine::messageCallback(const asSMessageInfo &msg)
{
    std::string type = "error";
    uint32_t logLevel = LogLevel::ERR;

    if(msg.type == asMSGTYPE_WARNING)
    {
        type = "warning";
        logLevel = LogLevel::WARN;
    }
    else if(msg.type == asMSGTYPE_INFORMATION)
    {
        type = "info";
        logLevel = LogLevel::INFO;
    }

    mBus.sendMessage<LogMessage>(LogMessage(msg.section + std::string(" (") + std::to_string(msg.row) + std::string(", ") + std::to_string(msg.col) + std::string(") : ") + type + std::string(" : ") + msg.message, "script", logLevel));

    if(msg.type == asMSGTYPE_ERROR)
    {
        //m_has_compile_errors = true;
    }
}

asIScriptEngine* ScriptEngine::getEngine()
{
    return mEngine;
}

asIScriptContext* ScriptEngine::requestContext()
{
    if(mContextsInUse == mContexts.size())
        mContexts.push_back(mEngine->CreateContext());

    mContextsInUse++;
    return mContexts[mContextsInUse - 1];
}

void ScriptEngine::freeContext(asIScriptContext* context)
{
    mContextsInUse--;
    context->Unprepare();
}
