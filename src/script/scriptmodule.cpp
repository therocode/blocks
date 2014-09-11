#include "scriptmodule.hpp"
#include "scriptengine.hpp"
#include "asaddons/scriptbuilder.hpp"
#include <assert.h>
#include <iostream>

ScriptModule::ScriptModule(const std::string& name, ScriptEngine& engine) : 
    mName(name),
    mEngine(engine),
    mAsModule(nullptr)
{
    
}

ScriptModule::ScriptModule(const ScriptModule&& other) : 
    mName(other.mName),
    mEngine(other.mEngine),
    mAsModule(other.mAsModule)
{
}

const std::string& ScriptModule::getName()
{
    return mName;
}

bool ScriptModule::compileFromSourceList(const std::vector<std::string>& files)
{
    mHasErrors = false;

    if(mAsModule != nullptr)
    {
        mEngine.destroyModule(*this);
    }

    CScriptBuilder builder;
    
    int32_t r = builder.StartNewModule(mEngine.getEngine(), mName.c_str()); assert(r >= 0);

    for(auto& path : files)
    {
        r = builder.AddSectionFromFile(path.c_str());
        if( r < 0 )
        {
            // The builder wasn't able to load the file. Maybe the file
            // has been removed, or the wrong name was given, or some
            // preprocessing commands are incorrectly written.
            std::cout << "Error loading script file " << path << "\n";
            mHasErrors = true;
            return false;
        }
    }

    r = builder.BuildModule();
    if( r < 0 )
    {
        mHasErrors = true;
        return false;
    }
     
    mAsModule = mEngine.getEngine()->GetModule(mName.c_str());
    return true;
}

asIScriptFunction* ScriptModule::getFunctionByDecl(const std::string& decl)
{
    return mAsModule->GetFunctionByDecl(decl.c_str());
}

asIObjectType* ScriptModule::getObjectTypeByDecl(const std::string& decl)
{
    if(!mAsModule)
        return nullptr;

    int32_t id = mAsModule->GetTypeIdByDecl(decl.c_str());

    if(id < 0)
    {
        return nullptr;
    }

    return mEngine.getEngine()->GetObjectTypeById(id);
}

std::vector<asIObjectType*> ScriptModule::getObjectTypes() const
{
    uint32_t objectCount = mAsModule->GetObjectTypeCount();
    std::vector<asIObjectType*> objectTypes;

    for(uint32_t i = 0; i < objectCount; i++)
    {
        objectTypes.push_back(mAsModule->GetObjectTypeByIndex(i));
    }

    return objectTypes;
}

bool ScriptModule::hasErrors() const
{
    return mHasErrors;
}
