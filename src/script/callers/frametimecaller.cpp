#include "frametimecaller.h"
#include <iostream>

FrameTimeCaller::FrameTimeCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptEntityMap& scriptEntities) : ScriptCaller(bus, engine, scriptEntities),
    frameTick(0)
{
    mBus.addSubscriber<FrameMessage>(*this);
}

FrameTimeCaller::~FrameTimeCaller()
{
    mBus.removeSubscriber<FrameMessage>(*this);
}

void FrameTimeCaller::handleMessage(const FrameMessage& received)
{
    if(mActive)
    {
        ScriptCallback<int32_t> onFrameCallback(mEngine);
        onFrameCallback.setFunction(mEngine.getEngine()->GetModule("scripts")->GetFunctionByDecl("void onFrame(int frameNumber)"));
        onFrameCallback.execute(frameTick);

        for(auto& entity : mScriptEntities)
        {
            ScriptMemberCallback<int32_t> callback(mEngine);
            asIScriptObject* object = entity.second.getScriptObject();
            asIScriptFunction* function = object->GetObjectType()->GetMethodByDecl("void onFrame(int frameNumber)");
            if(function)
            {
                callback.setFunction(function);
                callback.execute(object, frameTick);
            }
        }

        frameTick++;
    }
}
