#pragma once
#include "scriptinterface.hpp"

class ChunkInterface :
    public ScriptInterface
{
    public:
        ChunkInterface(fea::MessageBus& bus, GameInterface& worldInterface);
        
        void registerInterface(asIScriptEngine* engine) override;
};
