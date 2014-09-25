#pragma once
#include "../blockstd.hpp"
#include "../world/chunk.hpp"
#include "billboard.hpp"
#include "../input/inputmessages.hpp"
#include "renderingmessages.hpp"
#include "../entity/entitymessages.hpp"
#include "camera.hpp"
#include "../utilities/timer.hpp"
#include "vbocreator.hpp"

#include "shaderprogram.hpp"

#include "debugrenderer.hpp"
#include "frustum.hpp"

//#include "chunkgeneratorqueue.hpp"

class Renderer
    :   public fea::MessageReceiver<UpdateChunkVboMessage,
                                    ClientChunkDeletedMessage,
                                    WindowResizeMessage,
                                    AddGfxEntityMessage,
                                    MoveGfxEntityMessage,
                                    RotateGfxEntityMessage,
                                    PlayerFacingBlockMessage,
                                    RemoveGfxEntityMessage,
                                    PlayerIdMessage,
                                    PlayerConnectedToEntityMessage>
                
{
    public:
		static DebugRenderer sDebugRenderer;
        Renderer(fea::MessageBus& messageBus);
        
        //temporary thingy
      
        void makeTexture(const std::string& path, uint32_t width, uint32_t height, GLuint& textureId);
        void setup();
        virtual void handleMessage(const UpdateChunkVboMessage& received);
        virtual void handleMessage(const ClientChunkDeletedMessage& received);
        virtual void handleMessage(const WindowResizeMessage& received);
        virtual void handleMessage(const AddGfxEntityMessage& received);
        virtual void handleMessage(const MoveGfxEntityMessage& received);
        virtual void handleMessage(const RotateGfxEntityMessage& received);
        virtual void handleMessage(const RemoveGfxEntityMessage& received);
        virtual void handleMessage(const PlayerFacingBlockMessage& received);
        virtual void handleMessage(const PlayerIdMessage& received);
        virtual void handleMessage(const PlayerConnectedToEntityMessage& received);
        void render();
		void setCameraMatrix(const glm::mat4& m);
    private:
        void cameraUpdate();    // camera function
		//for camera animation
		float interpDuck = 0.0f;
		float duck = 0.f;
		glm::vec3 lastVel;
		glm::vec2 speedVec;
		float 	  speed = 0;
		
		ShaderProgram mShaderProgram;
		Camera cam;
		Timer mTimer;
		glm::vec3 originalCameraPos;
        glm::vec3 cameraOffset;
		glm::vec3 mCurrentlyFacingBlock;
		glm::vec3 mLastFacingBlock;
		float fadeIn = 0.f;
		glm::mat4 projectionMatrix;
        fea::MessageBus& bus;
        std::unordered_map<ChunkCoord, VBO> vbos;
        //ChunkGeneratorQueue mGeneratorQueue;
        std::unordered_map<size_t, Billboard> billboards;
        GLuint blockTexture;
        glm::vec2 mScreenSize;
        // camera stuff
        glm::vec3 mCameraPosition;

        size_t mPlayerId;

        size_t mCameraEntity;
        float mCameraPitch;
        float mCameraYaw;

        Frustum mFrustum;
        VBOCreator vboCreator;
};
