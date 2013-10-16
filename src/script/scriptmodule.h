#pragma once
#include <angelscript.h>
#include <string>

class ScriptModule
{
    public:
        ScriptModule(const std::string& name, asIScriptModule* module);
        ScriptModule(const ScriptModule&& other);
        const std::string& getName();
        void addScriptSection(const std::string& name, const std::string& source);
        void compileScripts();
        asIScriptFunction* getFunctionByDecl(const std::string& decl);
    private:
        const std::string mName;
        asIScriptModule* mAsModule;
};
