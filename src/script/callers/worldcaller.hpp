#pragma once
#include "scriptcaller.hpp"
#include "../../entity/entitymessages.hpp"
#include "../scriptcallback.hpp"

class WorldCaller :
	public ScriptCaller,
	public fea::MessageReceiver<EntityTransferringWorldMessage>
{
	public:
		WorldCaller(fea::MessageBus& bus, 
						 ScriptEngine& engine, 
						 ScriptEntityMap& scriptEntities);

		void handleMessage(const EntityTransferringWorldMessage& received);
};

