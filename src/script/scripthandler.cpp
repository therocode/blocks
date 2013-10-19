#include "scripthandler.h"
#include "../messages.h"
#include <iostream>

ScriptHandler::ScriptHandler(fea::MessageBus& bus, WorldInterface& worldInterface) : 
    mBus(bus),
    mScripts(mEngine.createModule("scripts")),
    mScriptInterface(mBus, mEngine, mScripts, worldInterface),
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
    mEngine.destroyModule(mScripts);
    mBus.removeMessageSubscriber<RebuildScriptsRequestedMessage>(*this);
    mBus.removeMessageSubscriber<EntityNeedsScriptMessage>(*this);
    mBus.removeMessageSubscriber<ScriptEntityFinishedMessage>(*this);
    mBus.removeMessageSubscriber<RemoveScriptEntityMessage>(*this);
    mBus.removeMessageSubscriber<EntityRemovedMessage>(*this);
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

void ScriptHandler::handleMessage(const EntityNeedsScriptMessage& message)
{
    fea::WeakEntityPtr entity;
    std::string scriptType;

    std::tie(entity, scriptType) = message.data;
    size_t id = entity.lock()->getId();
    asIScriptObject* obj = mScriptInterface.instanciateScriptEntity(scriptType);

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
