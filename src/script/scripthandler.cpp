#include "scripthandler.h"
#include <iostream>

ScriptHandler::ScriptHandler() : mScripts(mEngine.createModule("scripts"))
{
}

void ScriptHandler::setup()
{
    std::cout << "\nCompiling scripts...\n";

    std::string source = "void kork() {\nint kalle = 234;\nint roger = kalle;\nprint(\"hejhej\");}";
    
    mScripts.addScriptSection("kloss.as", source);   
    mScripts.compileScripts();   
    std::cout << "Compilation process over.\n\n";
}

void ScriptHandler::destroy()
{
}

ScriptHandler::~ScriptHandler()
{
    mEngine.destroyModule(mScripts);
}
