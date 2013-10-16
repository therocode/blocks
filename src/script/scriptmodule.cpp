#include "scriptmodule.h"

ScriptModule::ScriptModule(const std::string& name, asIScriptModule* module) : mName(name), mAsModule(module)
{
    
}

ScriptModule::ScriptModule(const ScriptModule&& other) : mName(other.mName), mAsModule(other.mAsModule)
{
}

const std::string& ScriptModule::getName()
{
    return mName;
}

void ScriptModule::addScriptSection(const std::string& name, const std::string& source)
{
    int r = mAsModule->AddScriptSection(name.c_str(), &source[0], source.size());
}

void ScriptModule::compileScripts()
{
    mAsModule->Build();
}
