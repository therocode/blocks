#include "scriptinterface.hpp"

ScriptInterface::ScriptInterface(fea::MessageBus& bus, WorldInterface& worldInterface) : 
    mBus(bus),
    mWorldInterface(worldInterface),
    mLogName("script")
{
}

ScriptInterface::~ScriptInterface()
{
}
