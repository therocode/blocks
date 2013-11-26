#pragma once
#include "scriptinterface.h"
#include "utilities/random.h"

class RandomInterface : public ScriptInterface
{
    public:
        void registerInterface(asIScriptEngine* engine) override;
    private:
        Random random;
};
