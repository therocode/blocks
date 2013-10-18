#include "scripthandler.h"
#include "../messages.h"
#include <iostream>

ScriptHandler::ScriptHandler(fea::MessageBus& bus) : 
    mBus(bus),
    mScripts(mEngine.createModule("scripts")),
    mScriptInterface(mBus, mEngine, mScripts)
{
    mBus.addMessageSubscriber<RebuildScriptsRequestedMessage>(*this);
    mBus.addMessageSubscriber<EntitySpawnedMessage>(*this);
}

ScriptHandler::~ScriptHandler()
{
    mEngine.destroyModule(mScripts);
    mBus.removeMessageSubscriber<RebuildScriptsRequestedMessage>(*this);
    mBus.removeMessageSubscriber<EntitySpawnedMessage>(*this);
}

void ScriptHandler::setup()
{
    mScriptInterface.registerInterface();

    sourceFiles = {"data/scripts/general.as", "data/scripts/entity.as"};
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

void ScriptHandler::handleMessage(const EntitySpawnedMessage& message)
{
    fea::WeakEntityPtr entity;
    std::string scriptType;

    std::tie(entity, scriptType) = message.data;
    size_t id = entity.lock()->getId();

    ScriptEntity scriptEntity(id, entity, mScriptInterface.instanciateScriptEntity(scriptType));

    scriptEntities.emplace(id, scriptEntity);
}
