#include "scriptcaller.h"

ScriptCaller::ScriptCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptEntityMap& scriptEntities) : mBus(bus), mEngine(engine), mActive(false), mScriptEntities(scriptEntities)
{
}

ScriptCaller::~ScriptCaller()
{
}

void ScriptCaller::setActive(bool active)
{
    mActive = active;
}
