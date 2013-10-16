#include "scriptengine.h"
#include <iostream>
#include <assert.h>

ScriptEngine::ScriptEngine()
{
    mEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

    int32_t r = mEngine->SetMessageCallback(asMETHOD(ScriptEngine, messageCallback), this, asCALL_THISCALL); assert( r >= 0 );
};

ScriptEngine::~ScriptEngine()
{
    mEngine->Release();
}

ScriptModule ScriptEngine::createModule(const std::string& name)
{
    return ScriptModule(name, mEngine->GetModule(name.c_str(), asGM_CREATE_IF_NOT_EXISTS));
}

void ScriptEngine::destroyModule(ScriptModule& module)
{
     mEngine->DiscardModule(module.getName().c_str());
}

void ScriptEngine::messageCallback(const asSMessageInfo &msg)
{
    const char *type = "error";
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
