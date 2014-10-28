#include "../catch.hpp"

#include "../src/script/scriptchunk.hpp"
#include "../src/script/asaddons/scriptarray.hpp"
#include "../src/world/chunk.hpp"
#include <angelscript.h>
#include <fea/assert.hpp>
#include <algorithm>

SCENARIO("Reading and writing voxel data to and from ScriptChunk causes corresponding changes to contained Chunk", "[set][get]")
{
	GIVEN("An AngelScript environment")
	{
		asIScriptEngine *engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		RegisterScriptArray(engine, false);
		asIObjectType *voxelDataArrayType = engine->GetObjectTypeByDecl("array<uint16>");
		
		
		GIVEN("A newly created chunk wrapped in a read/write ScriptChunk")
		{
			Chunk wrappedChunk;
			ScriptChunk scriptChunk(&wrappedChunk, voxelDataArrayType);
			
			WHEN("You get a single voxel's type through the ScriptChunk")
			{
				VoxelType scriptChunkType = scriptChunk.getVoxelType({0, 0, 0});
				
				THEN("The type is the same as the one you get through the Chunk")
				{
					VoxelType chunkType = wrappedChunk.getVoxelType({0, 0, 0});
					
					CHECK(scriptChunkType == chunkType);
				}
			}
			
			WHEN("A voxel is set to a particular value through the ScriptChunk")
			{
				scriptChunk.setVoxelType({0, 0, 0}, 1);
				
				THEN("The change is applied to the wrapped Chunk")
				{
					CHECK(1 == wrappedChunk.getVoxelType({0, 0, 0}));
				}
			}
			
			WHEN("A voxel is set to a particular value in the wrapped Chunk")
			{
				wrappedChunk.setVoxelType({0, 0, 0}, 1);
				
				THEN("The same value can be retrieved through the ScriptChunk")
				{
					CHECK(1 == scriptChunk.getVoxelType({0, 0, 0}));
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
						CHECK(wrappedChunk.getVoxelType({10, 1, 2}) == 1);
						CHECK(wrappedChunk.getVoxelType({4, 2, 10}) == 2);
						CHECK(wrappedChunk.getVoxelType({5, 9, 4})  == 3);
						CHECK(wrappedChunk.getVoxelType({15, 15, 15})  == 0);
					}
				}
			}
			
			GIVEN("Some voxel types set in the wrapped chunk")
			{
				wrappedChunk.setVoxelType({10, 1, 2}, 1);
				wrappedChunk.setVoxelType({4, 2, 10}, 2);
				wrappedChunk.setVoxelType({5, 9, 4},  3);
				
				THEN("Retrieving them in array form through the wrapped chunk and the ScriptChunk yields the same result")
				{
					VoxelTypeArray wrappedVoxelTypeArray = wrappedChunk.getFlatVoxelTypeData();
					
					VoxelTypeArray scriptVoxelTypeArray;
					CScriptArray* csVoxelTypeArray = scriptChunk.getVoxelData();
					for(int i = 0; i < chunkWidthPow3; i++)
						scriptVoxelTypeArray[i] = *(VoxelType*)csVoxelTypeArray->At(i);
					
					CHECK(std::equal(scriptVoxelTypeArray.begin(), scriptVoxelTypeArray.end(), wrappedVoxelTypeArray.begin()));
				}
			}
		}
		
		engine->Release();
	}
}