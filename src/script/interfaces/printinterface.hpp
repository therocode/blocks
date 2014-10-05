#pragma once
#include "scriptinterface.hpp"
#include "../../utilities/random.hpp"

class PrintInterface : public ScriptInterface
{
    public:
        PrintInterface(fea::MessageBus& bus, GameInterface& worldInterface);
        void registerInterface(asIScriptEngine* engine) override;
    private:
        void scriptPrint(const std::string& text);
        void scriptPrint(const std::string& text, uint32_t level);
		void scriptPrint(const std::string& text, const std::string& tag);
		void scriptPrint(const std::string& text, const std::string& tag, uint32_t level);
};
