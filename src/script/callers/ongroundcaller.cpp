#include "ongroundcaller.hpp"

OnGroundCaller::OnGroundCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptEntityMap& scriptEntities) : ScriptCaller(bus, engine, scriptEntities)
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
            ScriptMemberCallback<bool> callback(mEngine);
            asIScriptObject* object = entity->second.getScriptObject();
            asIScriptFunction* function = object->GetObjectType()->GetMethodByDecl("void onGround(bool landed)");
            if(function)
            {
                callback.setFunction(function);
                callback.execute(object, landed);
            }
        }
    }
}
