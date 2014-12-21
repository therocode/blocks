#include "scriptcaller.hpp"

ScriptCaller::ScriptCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptEntityMap& scriptEntities, std::vector<asIScriptObject*>& extensions) : mBus(bus), mEngine(engine), mActive(false), mScriptEntities(scriptEntities), mExtensions(extensions)
{
}

ScriptCaller::~ScriptCaller()
{
}

void ScriptCaller::setActive(bool active)
{
    mActive = active;
}
