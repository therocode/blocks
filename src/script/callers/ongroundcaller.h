#include "scriptcaller.h"
#include "entity/entitymessages.h"
#include "../scriptcallback.h"

class OnGroundCaller : 
    public ScriptCaller,
    public fea::MessageReceiver<EntityOnGroundMessage>
{
    public:
        OnGroundCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptEntityMap& scriptEntities);
        ~OnGroundCaller() override;
        void handleMessage(const EntityOnGroundMessage& received);
};
