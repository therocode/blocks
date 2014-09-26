#include "scriptinterface.hpp"

ScriptInterface::ScriptInterface(fea::MessageBus& bus, WorldInterface& worldInterface) : 
    mBus(bus),
    mWorldInterface(worldInterface)
{
}

ScriptInterface::~ScriptInterface()
{
}
