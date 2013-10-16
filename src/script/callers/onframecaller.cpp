#include "onframecaller.h"
#include <iostream>

OnFrameCaller::OnFrameCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptModule& module) : 
    mBus(bus),
    mEngine(engine), 
    mModule(module),
    mOnFrameFunc(nullptr)
{
    mBus.addMessageSubscriber<FrameMessage>(*this);
}

OnFrameCaller::~OnFrameCaller()
{
    mBus.removeMessageSubscriber<FrameMessage>(*this);
}

void OnFrameCaller::initialise()
{
    if(!mModule.hasErrors())
    {
        mOnFrameFunc = mModule.getFunctionByDecl("void onFrame()");
    }
}

void OnFrameCaller::handleMessage(const FrameMessage& received)
{
    if(!mModule.hasErrors())
    {
        asIScriptContext* context = mEngine.getContext();

        if(mOnFrameFunc)
        {
            int r = context->Prepare(mOnFrameFunc); assert( r >= 0 );
            r = context->Execute(); assert( r >= 0 );
            r = context->Unprepare(); assert( r >= 0 );
        }
    }
}
