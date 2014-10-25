#pragma once
#include "scriptcaller.hpp"
#include "../../world/worldmessages.hpp"
#include "../scriptcallback.hpp"
#include "../../world/chunk.hpp"
#include "../scriptchunk.hpp"

class ChunkEventCaller :
	public ScriptCaller,
	public fea::MessageReceiver<ChunkInitiallyGeneratedMessage, 
								ChunkCandidateMessage, 
								ChunkFinishedMessage>
{
	public:
		ChunkEventCaller(fea::MessageBus& bus, 
						 ScriptEngine& engine, 
						 ScriptEntityMap& scriptEntities);

		void handleMessage(const ChunkInitiallyGeneratedMessage& received);
		void handleMessage(const ChunkCandidateMessage& received);
		void handleMessage(const ChunkFinishedMessage& received);


};

