#include "printinterface.hpp"
#include <angelscript.h>
#include <assert.h>
#include "../../lognames.hpp"

PrintInterface::PrintInterface(fea::MessageBus& bus, WorldInterface& worldInterface) : ScriptInterface(bus, worldInterface)
{
}

void PrintInterface::registerInterface(asIScriptEngine* engine)
{
    int32_t r = engine->RegisterGlobalFunction("void consolePrint(string text)", asMETHODPR(PrintInterface, scriptPrint, (const std::string&), void), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void consolePrint(string text, uint level)", asMETHODPR(PrintInterface, scriptPrint, (const std::string&, uint32_t), void), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void consolePrint(string text, string text)", asMETHODPR(PrintInterface, scriptPrint, (const std::string&, const std::string&), void), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void consolePrint(string text, string text, uint level)", asMETHODPR(PrintInterface, scriptPrint, (const std::string&, const std::string&, uint32_t), void), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
}

void PrintInterface::scriptPrint(const std::string& text)
{
    mBus.send(LogMessage{text, scriptName, LogLevel::INFO});
}

void PrintInterface::scriptPrint(const std::string& text, uint32_t level)
{
    mBus.send(LogMessage{text, scriptName, level});
}

void PrintInterface::scriptPrint(const std::string& text, const std::string& tag)
{
    mBus.send(LogMessage{text, tag, LogLevel::INFO});
}

void PrintInterface::scriptPrint(const std::string& text, const std::string& tag, uint32_t level)
{
    mBus.send(LogMessage{text, tag, level});
}
