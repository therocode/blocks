#pragma once
#include <angelscript.h>
#include <string>
#include <vector>

class ScriptEngine;

class ScriptModule
{
    public:
        ScriptModule(const std::string& name, ScriptEngine& engine);
        ScriptModule(const ScriptModule&& other);
        const std::string& getName();
        void compileFromSourceList(const std::vector<std::string>& files);
        asIScriptFunction* getFunctionByDecl(const std::string& decl);
        bool hasErrors() const;
    private:
        const std::string mName;
        ScriptEngine& mEngine;
        asIScriptModule* mAsModule;
        bool mHasErrors;
};
