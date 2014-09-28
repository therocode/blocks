#include "ongroundcaller.hpp"

OnGroundCaller::OnGroundCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptEntityMap& scriptEntities) :
    ScriptCaller(bus, engine, scriptEntities),
    mCallback(mEngine)
{
    subscribe(mBus, *this);
}

void OnGroundCaller::handleMessage(const EntityOnGroundMessage& received)
{
    if(mActive)
    {
        bool landed = received.landed;
        size_t id = received.id;

        auto entity = mScriptEntities.find(id);

        if(entity != mScriptEntities.end())
        {
            asIScriptObject* object = entity->second.getScriptObject();

            asIScriptFunction* function = object->GetObjectType()->GetMethodByDecl("void onGround(bool landed)");
            if(function)
            {
                mCallback.setFunction(function);
                mCallback.execute(object, landed);
            }
        }
    }
}
