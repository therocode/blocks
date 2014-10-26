#include "worldcaller.hpp"

WorldCaller::WorldCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptEntityMap& scriptEntities) :
	ScriptCaller(bus, engine, scriptEntities)
{
	subscribe(mBus, *this);
}

void WorldCaller::handleMessage(const EntityTransferringWorldMessage& received)
{
	if(mActive) 
	{
		FEA_ASSERT(mScriptEntities.count(received.entityId) > 0, "Non existing script entity trying to change world!");
		ScriptCallback<WorldId, WorldId, fea::EntityId> scriptCallback(mEngine);
		scriptCallback.setFunction(mEngine.getEngine()->GetModule("scripts")->GetFunctionByDecl("bool entityEntersWorld(uint32, uint32, uint32)"));
		scriptCallback.execute(received.oldWorldId, received.newWorldId, received.entityId);		
	}
}
