#include "chunkeventcaller.hpp"

ChunkEventCaller::ChunkEventCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptEntityMap& scriptEntities, std::vector<asIScriptObject*>& extensions) :
    ScriptCaller(bus, engine, scriptEntities, extensions), 
    mVoxelDataArrayType(engine.getEngine()->GetObjectTypeByDecl("array<uint16>"))
{
    subscribe(mBus, *this);
}

void ChunkEventCaller::handleMessage(const ChunkInitiallyGeneratedMessage& received) 
{
    if(mActive) 
    {
        ScriptChunk chunk(&(received.chunk), mVoxelDataArrayType);    

        asIScriptContext *context = mEngine.requestContext();

        for(asIScriptObject* extensionObj : mExtensions)
        {
            asIObjectType* extensionType = extensionObj->GetObjectType();
            asIScriptFunction *function = extensionType->GetMethodByDecl("void chunkInitiallyGenerated(const uint32, const ChunkCoord &in, Chunk &in)");

            if(function)
            {
                ScriptMemberCallback<WorldId, ChunkCoord&, ScriptChunk&> callback(mEngine);
                callback.setFunction(function);
                callback.execute(extensionObj, received.worldId, (ChunkCoord&)received.coordinate, chunk);
            }
        }

        mEngine.freeContext(context);
    }
}

void ChunkEventCaller::handleMessage(const ChunkCandidateMessage& received) 
{
    if(mActive) 
    {
        ScriptChunk chunk(&(received.chunk), mVoxelDataArrayType);    

        asIScriptContext *context = mEngine.requestContext();

        for(asIScriptObject* extensionObj : mExtensions)
        {
            asIObjectType* extensionType = extensionObj->GetObjectType();
            asIScriptFunction *function = extensionType->GetMethodByDecl("void chunkCandidate(const uint32, const ChunkCoord &in, Chunk &in, const uint64)");

            if(function)
            {
                ScriptMemberCallback<WorldId, ChunkCoord&, ScriptChunk&, uint64_t> callback(mEngine);
                callback.setFunction(function);
                callback.execute(extensionObj, received.worldId, (ChunkCoord&)received.coordinate, chunk, received.frameNumberDelta);        
            }
        }

        mEngine.freeContext(context);
    }
}

void ChunkEventCaller::handleMessage(const ChunkFinishedMessage& received) 
{
    if(mActive) 
    {
         ScriptChunk chunk(&(received.chunk), mVoxelDataArrayType);    

        asIScriptContext *context = mEngine.requestContext();

        for(asIScriptObject* extensionObj : mExtensions)
        {
            asIObjectType* extensionType = extensionObj->GetObjectType();
            asIScriptFunction *function = extensionType->GetMethodByDecl("void chunkFinished(const uint32, const ChunkCoord &in, const Chunk &in)");

            if(function)
            {
                ScriptMemberCallback<WorldId, ChunkCoord&, ScriptChunk&> callback(mEngine);
                callback.setFunction(function);
                callback.execute(extensionObj, received.worldId, (ChunkCoord&)received.coordinate, chunk);
            }
        }

        mEngine.freeContext(context);
    }
}
