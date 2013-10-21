#include "scripthandler.h"
#include "scriptmessages.h"
#include "../entity/entitymessages.h"

ScriptHandler::ScriptHandler(fea::MessageBus& bus, WorldInterface& worldInterface) : 
    mBus(bus),
    mScripts(mEngine.createModule("scripts")),
    mScriptInterface(mBus, mEngine, mScripts, worldInterface, scriptEntityIds),
    mWorldInterface(worldInterface),
    logName("script")
{
    mBus.addMessageSubscriber<RebuildScriptsRequestedMessage>(*this);
    mBus.addMessageSubscriber<EntityNeedsScriptMessage>(*this);
    mBus.addMessageSubscriber<ScriptEntityFinishedMessage>(*this);
    mBus.addMessageSubscriber<RemoveScriptEntityMessage>(*this);
    mBus.addMessageSubscriber<EntityRemovedMessage>(*this);
}

ScriptHandler::~ScriptHandler()
{
    mBus.removeMessageSubscriber<RebuildScriptsRequestedMessage>(*this);
    mBus.removeMessageSubscriber<EntityNeedsScriptMessage>(*this);
    mBus.removeMessageSubscriber<ScriptEntityFinishedMessage>(*this);
    mBus.removeMessageSubscriber<RemoveScriptEntityMessage>(*this);
    mBus.removeMessageSubscriber<EntityRemovedMessage>(*this);
}

void ScriptHandler::setup()
{
    mEngine.setup();
    mScriptInterface.registerInterface();

    sourceFiles = {"data/scripts/general.as", "data/scripts/entity.as"};

    mBus.sendMessage<LogMessage>(LogMessage("Compiling scripts...", logName));
    bool succeeded = mScripts.compileFromSourceList(sourceFiles);
    mBus.sendMessage<LogMessage>(LogMessage("Compilation process over.", logName));

    if(succeeded)
    {
        mBus.sendMessage<LogMessage>(LogMessage("Setting up script callbacks...", logName));
        mScriptInterface.registerCallbacks(scriptEntities);
        mBus.sendMessage<LogMessage>(LogMessage("Compilation process over.", logName));
        mBus.sendMessage<LogMessage>(LogMessage("Done setting up callbacks.", logName));
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
    mBus.sendMessage<LogMessage>(LogMessage("Compiling scripts...", logName));
    bool succeeded = mScripts.compileFromSourceList(sourceFiles);
    mBus.sendMessage<LogMessage>(LogMessage("Compilation process over.", logName));

    if(succeeded)
    {
        mBus.sendMessage<LogMessage>(LogMessage("Setting up script callbacks...", logName));
        mScriptInterface.registerCallbacks(scriptEntities);
        mBus.sendMessage<LogMessage>(LogMessage("Compilation process over.", logName));
        mBus.sendMessage<LogMessage>(LogMessage("Done setting up callbacks.", logName));
    }
}

void ScriptHandler::handleMessage(const EntityNeedsScriptMessage& message)
{
    fea::WeakEntityPtr entity;
    std::string scriptType;

    std::tie(entity, scriptType) = message.data;
    size_t id = entity.lock()->getId();
    asIScriptObject* obj = mScriptInterface.instanciateScriptEntity(scriptType, id);

    ScriptEntity scriptEntity(id, entity, obj);

    scriptEntities.emplace(id, scriptEntity);
    scriptEntityIds.emplace(obj, id);
}

void ScriptHandler::handleMessage(const ScriptEntityFinishedMessage& message)
{
    size_t id;
    asIScriptObject* obj;
    fea::WeakEntityPtr entity;

    std::tie(id, obj, entity) = message.data;

    ScriptEntity scriptEntity(id, entity, obj);

    scriptEntities.emplace(id, scriptEntity);
    scriptEntityIds.emplace(obj, id);
}

void ScriptHandler::handleMessage(const RemoveScriptEntityMessage& message)
{
    asIScriptObject* obj;
    std::tie(obj) = message.data;

    size_t id = scriptEntityIds.at(obj);
    scriptEntityIds.erase(obj);

    mBus.sendMessage<RemoveEntityMessage>(RemoveEntityMessage(id));
}

void ScriptHandler::handleMessage(const EntityRemovedMessage& message)
{
    size_t id;
    std::tie(id) = message.data;

    scriptEntities.erase(id);
}
