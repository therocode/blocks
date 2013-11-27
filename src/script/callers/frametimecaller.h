#include "scriptcaller.h"
#include "entity/entitymessages.h"
#include "../scriptcallback.h"
#include "application/applicationmessages.h"

class FrameTimeCaller : 
    public ScriptCaller,
    public fea::MessageReceiver<FrameMessage>
{
    public:
        FrameTimeCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptEntityMap& scriptEntities);
        ~FrameTimeCaller() override;
        void handleMessage(const FrameMessage& received);
    private:
        int32_t frameTick;
};
