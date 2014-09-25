#include "scriptcaller.hpp"
#include "entity/entitymessages.hpp"
#include "../scriptcallback.hpp"
#include "application/applicationmessages.hpp"

class FrameTimeCaller : 
    public ScriptCaller,
    public fea::MessageReceiver<FrameMessage>
{
    public:
        FrameTimeCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptEntityMap& scriptEntities);
        void handleMessage(const FrameMessage& received);
    private:
        int32_t frameTick;
};
