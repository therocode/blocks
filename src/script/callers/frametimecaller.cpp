#include "frametimecaller.hpp"

FrameTimeCaller::FrameTimeCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptEntityMap& scriptEntities) : ScriptCaller(bus, engine, scriptEntities),
    frameTick(0),
    mCallback(mEngine)
{
    subscribe(mBus, *this);
}

void FrameTimeCaller::handleMessage(const FrameMessage& received)
{
    if(mActive)
    {
        mCallback.setFunction(mEngine.getEngine()->GetModule("scripts")->GetFunctionByDecl("void onFrame(int frameNumber)"));
        mCallback.execute(frameTick);

        for(auto& entity : mScriptEntities)
        {
            asIScriptObject* object = entity.second.getScriptObject();
            asIScriptFunction* function = object->GetObjectType()->GetMethodByDecl("void onFrame(int frameNumber)");
            if(function)
            {
                mCallback.setFunction(function);
                mCallback.execute(object, frameTick);
            }
        }

        frameTick++;
    }
}
