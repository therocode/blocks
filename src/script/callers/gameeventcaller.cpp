#include "gameeventcaller.hpp"

GameEventCaller::GameEventCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptEntityMap& scriptEntities, std::vector<asIScriptObject*>& extensions) 
    : ScriptCaller(bus, engine, scriptEntities, extensions)
{
    subscribe(mBus, *this);
}

void GameEventCaller::handleMessage(const GameStartMessage& received)
{
    if(mActive)
    {
        ScriptCallback<> gameStartCallback(mEngine);
        gameStartCallback.setFunction(mEngine.getEngine()->GetModule("scripts")->GetFunctionByDecl("void gameStarted()"));
        gameStartCallback.execute();
    }
}
