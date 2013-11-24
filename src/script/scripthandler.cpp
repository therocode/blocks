#include "scripthandler.h"
#include "scriptmessages.h"
#include "../entity/entitymessages.h"
#include "../utilities/folderexploder.h"
#include <iostream>

ScriptHandler::ScriptHandler(fea::MessageBus& bus, WorldInterface& worldInterface) : 
    mEngine(bus),
    mBus(bus),
    mScripts(mEngine.createModule("scripts")),
    mScriptInterface(mBus, mEngine, mScripts, worldInterface, scriptEntityIds),
    mWorldInterface(worldInterface),
    logName("script")
{
    mBus.addMessageSubscriber<RebuildScriptsRequestedMessage>(*this);
    mBus.addMessageSubscriber<EntityCreatedMessage>(*this);
    mBus.addMessageSubscriber<EntityRemovedMessage>(*this);
}

ScriptHandler::~ScriptHandler()
{
    mBus.removeMessageSubscriber<RebuildScriptsRequestedMessage>(*this);
    mBus.removeMessageSubscriber<EntityCreatedMessage>(*this);
    mBus.removeMessageSubscriber<EntityRemovedMessage>(*this);
}

void ScriptHandler::setup()
{
    mEngine.setup();
    mScriptInterface.registerInterface();

    FolderExploder exploder;

    sourceFiles.clear();

    exploder.explodeFolder("data", ".*\\.as", sourceFiles);
    for(auto& string : sourceFiles)
    {
        mBus.sendMessage<LogMessage>(LogMessage("Adding " + string + " for compilation.", logName, LogLevel::VERB));
    }

    mBus.sendMessage<LogMessage>(LogMessage("Compiling scripts...", logName, LogLevel::INFO));
    bool succeeded = mScripts.compileFromSourceList(sourceFiles);
    mBus.sendMessage<LogMessage>(LogMessage("Compilation process over.", logName, LogLevel::INFO));

    if(succeeded)
    {
        mBus.sendMessage<LogMessage>(LogMessage("Setting up script callbacks...", logName, LogLevel::VERB));
        mScriptInterface.registerCallbacks(scriptEntities);
        mBus.sendMessage<LogMessage>(LogMessage("Compilation process over.", logName, LogLevel::VERB));
        mBus.sendMessage<LogMessage>(LogMessage("Done setting up callbacks.", logName, LogLevel::VERB));
    }
}

void ScriptHandler::destroy()
{
    scriptEntities.clear();
    scriptEntityIds.clear();
    mEngine.destroyModule(mScripts);
    mEngine.destroy();
}

void ScriptHandler::handleMessage(const RebuildScriptsRequestedMessage& message)
{
    mBus.sendMessage<LogMessage>(LogMessage("Compiling scripts...", logName, LogLevel::INFO));
    bool succeeded = mScripts.compileFromSourceList(sourceFiles);
    mBus.sendMessage<LogMessage>(LogMessage("Compilation process over.", logName, LogLevel::INFO));

    if(succeeded)
    {
        mBus.sendMessage<LogMessage>(LogMessage("Setting up script callbacks...", logName, LogLevel::VERB));
        mScriptInterface.registerCallbacks(scriptEntities);
        mBus.sendMessage<LogMessage>(LogMessage("Compilation process over.", logName, LogLevel::VERB));
        mBus.sendMessage<LogMessage>(LogMessage("Done setting up callbacks.", logName, LogLevel::VERB));
    }
}

void ScriptHandler::handleMessage(const EntityCreatedMessage& message)
{
    //tbi
}

void ScriptHandler::handleMessage(const EntityRemovedMessage& message)
{
    size_t id;
    std::tie(id) = message.data;

    scriptEntityIds.erase(scriptEntities.at(id).getScriptObject());
    scriptEntities.erase(id);
}
