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
        bool compileFromSourceList(const std::vector<std::string>& files);
        asIScriptFunction* getFunctionByDecl(const std::string& decl);
        asIObjectType* getObjectTypeByDecl(const std::string& decl);
        std::vector<asIObjectType*> getObjectTypes() const;
        bool hasErrors() const;
    private:
        std::string mName;
        ScriptEngine& mEngine;
        asIScriptModule* mAsModule;
        bool mHasErrors;
};
