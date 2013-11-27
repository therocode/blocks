#include "gameeventcaller.h"

GameEventCaller::GameEventCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptEntityMap& scriptEntities) : ScriptCaller(bus, engine, scriptEntities)
{
    mBus.addMessageSubscriber<GameStartMessage>(*this);
}

GameEventCaller::~GameEventCaller()
{
    mBus.removeMessageSubscriber<GameStartMessage>(*this);
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
