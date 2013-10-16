#include "scripthandler.h"
#include <iostream>

ScriptHandler::ScriptHandler() : mScripts(mEngine.createModule("scripts"))
{
}

void ScriptHandler::setup()
{
    std::cout << "\nCompiling scripts...\n";
    mScripts.addScriptSection("kloss.as", "int kalle = 234;\nint roger = kalle;");   
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
