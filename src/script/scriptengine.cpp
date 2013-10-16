#include "scriptengine.h"
#include <iostream>
#include <assert.h>
#include "asaddons/scriptstdstring.h"
#include "asaddons/scriptarray.h"

ScriptEngine::ScriptEngine()
{
    mEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
    mContext = mEngine->CreateContext();

    RegisterScriptArray(mEngine, true);
    RegisterStdString(mEngine);
    RegisterStdStringUtils(mEngine);

    int32_t r = mEngine->SetMessageCallback(asMETHOD(ScriptEngine, messageCallback), this, asCALL_THISCALL); assert( r >= 0 );
};

ScriptEngine::~ScriptEngine()
{
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
    if(msg.type == asMSGTYPE_WARNING)
    {
        type = "warning";
    }
    else if(msg.type == asMSGTYPE_INFORMATION)
    {
        type = "info";
    }

    std::cout << msg.section << " (" << msg.row << ", " << msg.col << ") : " << type << " : " << msg.message << std::endl;

    if(msg.type == asMSGTYPE_ERROR)
    {
        //m_has_compile_errors = true;
    }
}

asIScriptContext* ScriptEngine::getContext()
{
    return mContext;
}

asIScriptEngine* ScriptEngine::getEngine()
{
    return mEngine;
}
