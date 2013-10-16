#include "scripthandler.h"
#include <iostream>

ScriptHandler::ScriptHandler(fea::MessageBus& bus) : 
    mBus(bus),
    mScripts(mEngine.createModule("scripts")),
    mOnFrameCaller(mBus, mEngine, mScripts)
{
}

void ScriptHandler::setup()
{
    std::cout << "\nCompiling scripts...\n";

    std::string source = "void onFrame() {\nint kalle = 234;\nint roger = kalle;\nprint(\"hejhej\\n\");}";
    
    mScripts.addScriptSection("kloss.as", source);   
    mScripts.compileScripts();   
    std::cout << "Compilation process over.\n";

    std::cout << "Setting up script callers...\n";
    mOnFrameCaller.initialise();
    std::cout << "Done with callers!\n\n";

}

void ScriptHandler::destroy()
{
}

ScriptHandler::~ScriptHandler()
{
    mEngine.destroyModule(mScripts);
}
