#include "scriptinterface.hpp"

ScriptInterface::ScriptInterface(fea::MessageBus& bus, GameInterface& worldInterface) : 
    mBus(bus),
    mGameInterface(worldInterface)
{
}

ScriptInterface::~ScriptInterface()
{
}
