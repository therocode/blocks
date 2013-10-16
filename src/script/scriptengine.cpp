#include "scriptengine.h"

ScriptEngine::ScriptEngine()
{
    mEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
};

ScriptEngine::~ScriptEngine()
{
}
