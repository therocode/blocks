#include "scriptinterface.h"
#include <iostream>

ScriptInterface::ScriptInterface(fea::MessageBus& bus, ScriptEngine& engine, ScriptModule& module) : 
    mBus(bus),
    mEngine(engine), 
    mModule(module),
    mOnFrameFunc(nullptr)
{
    mBus.addMessageSubscriber<FrameMessage>(*this);
}

ScriptInterface::~ScriptInterface()
{
    mBus.removeMessageSubscriber<FrameMessage>(*this);
}

void ScriptInterface::registerInterface()
{
        int r = mEngine.getEngine()->RegisterGlobalFunction("void print(string text)", asMETHOD(ScriptInterface, scriptPrint), asCALL_THISCALL_ASGLOBAL, this); assert( r >= 0);
        r = mEngine.getEngine()->RegisterGlobalFunction("void setGravity(float constant)", asMETHOD(ScriptInterface, setGravity), asCALL_THISCALL_ASGLOBAL, this); assert( r >= 0);
}

void ScriptInterface::registerCallbacks()
{
    if(!mModule.hasErrors())
    {
        mOnFrameFunc = mModule.getFunctionByDecl("void onFrame()");
    }
}

void ScriptInterface::handleMessage(const FrameMessage& received)
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

void ScriptInterface::scriptPrint(std::string text)
{
    std::cout << text;
}

void ScriptInterface::setGravity(float constant)
{
    mBus.sendMessage<GravityRequestedMessage>(GravityRequestedMessage(constant));
}
