#include "ongroundcaller.h"

OnGroundCaller::OnGroundCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptEntityMap& scriptEntities) : ScriptCaller(bus, engine, scriptEntities)
{
    mBus.addSubscriber<EntityOnGroundMessage>(*this);
}

OnGroundCaller::~OnGroundCaller()
{
    mBus.removeSubscriber<EntityOnGroundMessage>(*this);
}

void OnGroundCaller::handleMessage(const EntityOnGroundMessage& received)
{
    if(mActive)
    {
        bool landed;
        size_t id;

        std::tie(id, landed) = received.data;

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
