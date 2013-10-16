#include "scripthandler.h"
#include "../messages.h"
#include <iostream>

ScriptHandler::ScriptHandler(fea::MessageBus& bus) : 
    mBus(bus),
    mScripts(mEngine.createModule("scripts")),
    mOnFrameCaller(mBus, mEngine, mScripts)
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
    sourceFiles = {"data/scripts/general.as"};
    std::cout << "\nCompiling scripts...\n";

    mScripts.compileFromSourceList(sourceFiles);
    std::cout << "Compilation process over.\n";

    std::cout << "Setting up script callers...\n";
    mOnFrameCaller.initialise();
    std::cout << "Done with callers!\n\n";

}

void ScriptHandler::destroy()
{
}

void ScriptHandler::handleMessage(const RebuildScriptsRequestedMessage& message)
{
    std::cout << "\nCompiling scripts...\n";
    mScripts.compileFromSourceList(sourceFiles);
    std::cout << "Compilation process over.\n";

    std::cout << "Setting up script callers...\n";
    mOnFrameCaller.initialise();
    std::cout << "Done with callers!\n\n";
}
