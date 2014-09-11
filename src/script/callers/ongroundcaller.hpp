#include "scriptcaller.hpp"
#include "entity/entitymessages.hpp"
#include "../scriptcallback.hpp"

class OnGroundCaller : 
    public ScriptCaller,
    public fea::MessageReceiver<EntityOnGroundMessage>
{
    public:
        OnGroundCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptEntityMap& scriptEntities);
        ~OnGroundCaller() override;
        void handleMessage(const EntityOnGroundMessage& received);
};
