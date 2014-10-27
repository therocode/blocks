#include "../catch.hpp"

#include "../src/script/scriptchunk.hpp"
#include "../src/script/asaddons/scriptarray.hpp"
#include "../src/world/chunk.hpp"
#include <angelscript.h>
#include <fea/assert.hpp>

SCENARIO("Reading and writing voxel data to and from ScriptChunk causes corresponding changes to contained Chunk", "[set][get]")
{
	GIVEN("An AngelScript environment")
	{
		asIScriptEngine *engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		RegisterScriptArray(engine, false);
		asIObjectType *voxelDataArrayType = engine->GetObjectTypeByDecl("array<uint16>");
		
		
		GIVEN("A newly created chunk wrapped in a read/write ScriptChunk")
		{
			Chunk chunk;
			ScriptChunk scriptChunk(&chunk, voxelDataArrayType);
			
			WHEN("You get a single voxel's type through the ScriptChunk")
			{
				VoxelType scriptChunkType = scriptChunk.getVoxelType({0, 0, 0});
				
				THEN("The type is the same as the one you get through the Chunk")
				{
					VoxelType chunkType = chunk.getVoxelType({0, 0, 0});
					
					CHECK(scriptChunkType == chunkType);
				}
			}
			
			WHEN("A voxel is set to a particular value through the ScriptChunk")
			{
				scriptChunk.setVoxelType({0, 0, 0}, 1);
				
				THEN("The change is applied to the wrapped Chunk")
				{
					CHECK(1 == chunk.getVoxelType({0, 0, 0}));
				}
			}
			
			GIVEN("Some voxel type data in array form")
			{
				Chunk arrayChunk;

				arrayChunk.setVoxelType({10, 1, 2}, 1);
				arrayChunk.setVoxelType({4, 2, 10}, 2);
				arrayChunk.setVoxelType({5, 9, 4},  3);
				
				VoxelTypeArray voxelTypeArray = arrayChunk.getFlatVoxelTypeData();

				CScriptArray* csVoxelTypeArray = CScriptArray::Create(voxelDataArrayType, chunkWidthPow3);
				
				for(int i = 0; i < chunkWidthPow3; i++)
					csVoxelTypeArray->SetValue(i, &voxelTypeArray[i]);
				
				WHEN("Writing the voxel type array to the ScriptChunk")
				{
					scriptChunk.setVoxelData(*csVoxelTypeArray);
					
					THEN("The wrapped chunk holds the same data")
					{
						CHECK(chunk.getVoxelType({10, 1, 2}) == 1);
						CHECK(chunk.getVoxelType({4, 2, 10}) == 2);
						CHECK(chunk.getVoxelType({5, 9, 4})  == 3);
						CHECK(chunk.getVoxelType({15, 15, 15})  == 0);
					}
				}
			}
		}
		
		engine->Release();
	}
}