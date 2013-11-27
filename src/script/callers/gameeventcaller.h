#include "scriptcaller.h"
#include "entity/entitymessages.h"
#include "../scriptcallback.h"
#include "application/applicationmessages.h"

class GameEventCaller : 
    public ScriptCaller,
    public fea::MessageReceiver<GameStartMessage>
{
    public:
        GameEventCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptEntityMap& scriptEntities);
        ~GameEventCaller() override;
        void handleMessage(const GameStartMessage& received);
};
