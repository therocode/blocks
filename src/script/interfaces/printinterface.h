#pragma once
#include "scriptinterface.h"
#include "utilities/random.h"

class PrintInterface : public ScriptInterface
{
    public:
        PrintInterface(fea::MessageBus& bus, WorldInterface& worldInterface);
        void registerInterface(asIScriptEngine* engine) override;
    private:
        void scriptPrint(const std::string& text);
        void scriptPrint(const std::string& text, uint32_t level);
};
