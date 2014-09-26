#pragma once
#include "scriptinterface.hpp"
#include "utilities/random.hpp"

class PrintInterface : public ScriptInterface
{
    public:
        PrintInterface(fea::MessageBus& bus, WorldInterface& worldInterface);
        void registerInterface(asIScriptEngine* engine) override;
    private:
        void scriptPrint(const std::string& text);
        void scriptPrint(const std::string& text, uint32_t level);
		void scriptPrint(const std::string& text, const std::string& tag);
};
