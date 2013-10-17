#include "scriptinterface.h"
#include <iostream>

ScriptInterface::ScriptInterface(fea::MessageBus& bus, ScriptEngine& engine, ScriptModule& module) : 
    mBus(bus),
    mEngine(engine), 
    mModule(module),
    onFrameCallback(engine),
    frameTick(0)
{
    mBus.addMessageSubscriber<FrameMessage>(*this);
}

ScriptInterface::~ScriptInterface()
{
    mBus.removeMessageSubscriber<FrameMessage>(*this);
}

void ScriptInterface::registerInterface()
{
        int r = mEngine.getEngine()->RegisterGlobalFunction("void consolePrint(string text)", asMETHOD(ScriptInterface, scriptPrint), asCALL_THISCALL_ASGLOBAL, this); assert( r >= 0);
        r = mEngine.getEngine()->RegisterGlobalFunction("void setGravity(float constant)", asMETHOD(ScriptInterface, setGravity), asCALL_THISCALL_ASGLOBAL, this); assert( r >= 0);
        r = mEngine.getEngine()->RegisterGlobalFunction("uint randomInt()", asMETHOD(Random, randomInt), asCALL_THISCALL_ASGLOBAL, &random); assert( r >= 0);
        r = mEngine.getEngine()->RegisterGlobalFunction("int randomIntRange(int start, int end)", asMETHOD(Random, randomIntRange), asCALL_THISCALL_ASGLOBAL, &random); assert( r >= 0);
}

void ScriptInterface::registerCallbacks()
{
    if(!mModule.hasErrors())
    {
        onFrameCallback.setFunction(mModule.getFunctionByDecl("void onFrame(int frameNumber)"));
    }
}

void ScriptInterface::handleMessage(const FrameMessage& received)
{
    if(!mModule.hasErrors())
    {
        asIScriptContext* context = mEngine.getContext();

        onFrameCallback.execute(frameTick);
        frameTick++;
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
