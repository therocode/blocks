#include "chunkeventcaller.hpp"

ChunkEventCaller::ChunkEventCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptEntityMap& scriptEntities) :
	ScriptCaller(bus, engine, scriptEntities)
{
	subscribe(mBus, *this);
}

void ChunkEventCaller::handleMessage(const ChunkInitiallyGeneratedMessage& received) 
{
	if(mActive) 
	{
		ScriptChunk chunk(&(received.chunk));	
		ScriptCallback<WorldId, ChunkCoord&, ScriptChunk&> scriptCallback(mEngine);
		scriptCallback.setFunction(mEngine.getEngine()->GetModule("scripts")->GetFunctionByDecl("void chunkInitiallyGenerated(const uint32, const ChunkCoord &in, Chunk &in)"));
		scriptCallback.execute(received.worldId, (ChunkCoord&)received.coordinate, chunk);		
	}
}

void ChunkEventCaller::handleMessage(const ChunkCandidateMessage& received) 
{
	if(mActive) 
	{
		ScriptChunk chunk(&(received.chunk));	
		ScriptCallback<WorldId, ChunkCoord&, ScriptChunk&> scriptCallback(mEngine);
		scriptCallback.setFunction(mEngine.getEngine()->GetModule("scripts")->GetFunctionByDecl("void chunkCandidate(const uint32, const ChunkCoord &in, Chunk &in)"));
		scriptCallback.execute(received.worldId, (ChunkCoord&)received.coordinate, chunk);		
	}
}

void ChunkEventCaller::handleMessage(const ChunkFinishedMessage& received) 
{
	if(mActive) 
	{
		ScriptChunk chunk(&(received.chunk));	
		ScriptCallback<WorldId, ChunkCoord&, ScriptChunk&> scriptCallback(mEngine);
		scriptCallback.setFunction(mEngine.getEngine()->GetModule("scripts")->GetFunctionByDecl("void chunkFinished(const uint32, const ChunkCoord &in, const Chunk &in)"));
		scriptCallback.execute(received.worldId, (ChunkCoord&)received.coordinate, chunk);
	}
}
