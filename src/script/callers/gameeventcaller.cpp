#include "gameeventcaller.hpp"

GameEventCaller::GameEventCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptEntityMap& scriptEntities) : ScriptCaller(bus, engine, scriptEntities)
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
