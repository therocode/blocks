#include "scriptcaller.hpp"
#include "../../entity/entitymessages.hpp"
#include "../scriptcallback.hpp"
#include "../../application/applicationmessages.hpp"

class GameEventCaller : 
    public ScriptCaller,
    public fea::MessageReceiver<GameStartMessage>
{
    public:
        GameEventCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptEntityMap& scriptEntities, std::vector<asIScriptObject*>& extensions);
        void handleMessage(const GameStartMessage& received);
};
