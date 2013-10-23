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

class Renderer
    :   public fea::MessageReceiver<ChunkCreatedMessage>,
        public fea::MessageReceiver<WindowResizeMessage>,
        public fea::MessageReceiver<PlayerActionMessage>,
        public fea::MessageReceiver<PlayerPitchYawMessage>,
        public fea::MessageReceiver<AddGfxEntityMessage>,
        public fea::MessageReceiver<MoveGfxEntityMessage>,
        public fea::MessageReceiver<CurrentlyFacingBlockMessage>,
        public fea::MessageReceiver<RemoveGfxEntityMessage>,
        public fea::MessageReceiver<PlayerConnectedToEntityMessage>
                
{
    public:
        Renderer(fea::MessageBus& messageBus);
        ~Renderer();
        void makeTexture(std::string path, uint32_t width, uint32_t height, GLuint& textureId);
        void setup();
        virtual void handleMessage(const ChunkCreatedMessage& received);
        virtual void handleMessage(const WindowResizeMessage& received);
        virtual void handleMessage(const PlayerActionMessage& received);
        virtual void handleMessage(const PlayerPitchYawMessage& received);
        virtual void handleMessage(const AddGfxEntityMessage& received);
        virtual void handleMessage(const MoveGfxEntityMessage& received);
        virtual void handleMessage(const RemoveGfxEntityMessage& received);
        virtual void handleMessage(const CurrentlyFacingBlockMessage& received);
        virtual void handleMessage(const PlayerConnectedToEntityMessage& received);
        void render();
        void cameraUpdate();    // camera function
		void setCameraMatrix(glm::mat4 m);
    private:
		ShaderProgram mShaderProgram;
		Camera cam;
		Timer mTimer;
		glm::vec3 mCurrentlyFacingBlock;
		glm::mat4 projectionMatrix;
        fea::MessageBus& bus;
        std::vector<VBO> vbos;
        std::map<size_t, Billboard> billboards;
        GLuint blockTexture;
        glm::vec2 mScreenSize;
        // camera stuff
		float moveSpeed = 0.0002f;
		float mTimeStep = 2.f;
		float lastY, lastX;
		bool movingLeft, movingRight, movingUp, movingDown, elevate, delevate;
		bool mouseDown = false;
		glm::vec3 camSpeed;

        size_t cameraEntity;
};
