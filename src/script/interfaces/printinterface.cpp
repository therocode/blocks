#include "printinterface.hpp"
#include <angelscript.h>
#include <assert.h>

PrintInterface::PrintInterface(fea::MessageBus& bus, WorldInterface& worldInterface) : ScriptInterface(bus, worldInterface)
{
}

void PrintInterface::registerInterface(asIScriptEngine* engine)
{
    int32_t r = engine->RegisterGlobalFunction("void consolePrint(string text)", asMETHODPR(PrintInterface, scriptPrint, (const std::string&), void), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void consolePrint(string text, uint level)", asMETHODPR(PrintInterface, scriptPrint, (const std::string&, uint32_t), void), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);

}

void PrintInterface::scriptPrint(const std::string& text)
{
    mBus.send(LogMessage{text, mLogName, LogLevel::INFO});
}

void PrintInterface::scriptPrint(const std::string& text, uint32_t level)
{
    mBus.send(LogMessage{text, mLogName, level});
}
