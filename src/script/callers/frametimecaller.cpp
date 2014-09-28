#include "frametimecaller.hpp"

FrameTimeCaller::FrameTimeCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptEntityMap& scriptEntities) : ScriptCaller(bus, engine, scriptEntities),
    frameTick(0),
    mMainCallback(mEngine),
    mEntityCallback(mEngine)
{
    subscribe(mBus, *this);
}

void FrameTimeCaller::handleMessage(const FrameMessage& received)
{
    if(mActive)
    {
        mMainCallback.setFunction(mEngine.getEngine()->GetModule("scripts")->GetFunctionByDecl("void onFrame(int frameNumber)"));
        mMainCallback.execute(frameTick);

        for(auto& entity : mScriptEntities)
        {
            asIScriptObject* object = entity.second.getScriptObject();
            asIScriptFunction* function = object->GetObjectType()->GetMethodByDecl("void onFrame(int frameNumber)");
            if(function)
            {
                mEntityCallback.setFunction(function);
                mEntityCallback.execute(object, frameTick);
            }
        }

        frameTick++;
    }
}
