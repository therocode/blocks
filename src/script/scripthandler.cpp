#include "scripthandler.h"
#include "../messages.h"
#include <iostream>

ScriptHandler::ScriptHandler(fea::MessageBus& bus) : 
    mBus(bus),
    mScripts(mEngine.createModule("scripts")),
    mScriptInterface(mBus, mEngine, mScripts)
{
    mBus.addMessageSubscriber<RebuildScriptsRequestedMessage>(*this);
}

ScriptHandler::~ScriptHandler()
{
    mEngine.destroyModule(mScripts);
    mBus.removeMessageSubscriber<RebuildScriptsRequestedMessage>(*this);
}

void ScriptHandler::setup()
{
    mScriptInterface.registerInterface();

    sourceFiles = {"data/scripts/general.as"};
    std::cout << "\nCompiling scripts...\n";

    bool succeeded = mScripts.compileFromSourceList(sourceFiles);
    std::cout << "Compilation process over.\n";

    if(succeeded)
    {
        std::cout << "Setting up script callbacks...\n";
        mScriptInterface.registerCallbacks();
        std::cout << "Done with callbacks!\n\n";
    }

}

void ScriptHandler::destroy()
{
}

void ScriptHandler::handleMessage(const RebuildScriptsRequestedMessage& message)
{
    std::cout << "\nCompiling scripts...\n";
    mScripts.compileFromSourceList(sourceFiles);
    std::cout << "Compilation process over.\n";

    std::cout << "Setting up script callbacks...\n";
    mScriptInterface.registerCallbacks();
    std::cout << "Done with callbacks!\n\n";
}
