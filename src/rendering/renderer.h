#pragma once
#include "../blockstd.h"
#include "../world/chunk.h"
#include "vbo.h"
#include "billboard.h"
#include "../input/inputmessages.h"
#include "renderingmessages.h"
#include "../entity/entitymessages.h"
#include "../world/worldmessages.h"
#include "camera.h"
#include <vector>
#include <map>
#include "../utilities/timer.h"

#include "shaderprogram.h"

#include "debugrenderer.h"

class Renderer
    :   public fea::MessageReceiver<ChunkCreatedMessage>,
        public fea::MessageReceiver<ChunkDeletedMessage>,
        public fea::MessageReceiver<WindowResizeMessage>,
        public fea::MessageReceiver<AddGfxEntityMessage>,
        public fea::MessageReceiver<MoveGfxEntityMessage>,
        public fea::MessageReceiver<RotateGfxEntityMessage>,
        public fea::MessageReceiver<PlayerFacingBlockMessage>,
        public fea::MessageReceiver<RemoveGfxEntityMessage>,
        public fea::MessageReceiver<PlayerIdMessage>,
        public fea::MessageReceiver<PlayerConnectedToEntityMessage>
                
{
    public:
		static DebugRenderer sDebugRenderer;
        Renderer(fea::MessageBus& messageBus);
        ~Renderer();
        void makeTexture(const std::string& path, uint32_t width, uint32_t height, GLuint& textureId);
        void setup();
        virtual void handleMessage(const ChunkCreatedMessage& received);
        virtual void handleMessage(const ChunkDeletedMessage& received);
        virtual void handleMessage(const WindowResizeMessage& received);
        virtual void handleMessage(const AddGfxEntityMessage& received);
        virtual void handleMessage(const MoveGfxEntityMessage& received);
        virtual void handleMessage(const RotateGfxEntityMessage& received);
        virtual void handleMessage(const RemoveGfxEntityMessage& received);
        virtual void handleMessage(const PlayerFacingBlockMessage& received);
        virtual void handleMessage(const PlayerIdMessage& received);
        virtual void handleMessage(const PlayerConnectedToEntityMessage& received);
        void render();
        void cameraUpdate();    // camera function
		void setCameraMatrix(const glm::mat4& m);
    private:
		ShaderProgram mShaderProgram;
		Camera cam;
		Timer mTimer;
		glm::vec3 originalCameraPos;
		glm::vec3 mCurrentlyFacingBlock;
		glm::vec3 mLastFacingBlock;
		float fadeIn = 0.f;
		glm::mat4 projectionMatrix;
        fea::MessageBus& bus;
        std::unordered_map<ChunkCoordinate, VBO> vbos;
        std::unordered_map<size_t, Billboard> billboards;
        GLuint blockTexture;
        glm::vec2 mScreenSize;
        // camera stuff
        glm::vec3 mCameraPosition;

        size_t mPlayerId;

        size_t mCameraEntity;
        float mCameraPitch;
        float mCameraYaw;
};
