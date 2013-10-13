#pragma once
#include "blockstd.h"
#include "chunk.h"
#include "vbo.h"
#include "billboard.h"
#include "messages.h"
#include "camera.h"
#include <vector>
#include <featherkit/messaging.h>
#include "timer.h"

#include "shaderprogram.h"

class Renderer
    :   public fea::MessageReceiver<ChunkCreatedMessage>,
        public fea::MessageReceiver<WindowResizeMessage>,
        public fea::MessageReceiver<InputActionMessage>,
        public fea::MessageReceiver<MouseMovedMessage>,
        public fea::MessageReceiver<AddGfxEntityMessage>,
        public fea::MessageReceiver<MoveGfxEntityMessage>,
        public fea::MessageReceiver<RemoveGfxEntityMessage>
                
{
    public:
        Renderer(fea::MessageBus& messageBus);
        ~Renderer();
        void makeTexture(std::string path, uint32_t width, uint32_t height, GLuint& textureId);
        void setup();
        virtual void handleMessage(const ChunkCreatedMessage& received);
        virtual void handleMessage(const WindowResizeMessage& received);
        virtual void handleMessage(const InputActionMessage& received);
        virtual void handleMessage(const MouseMovedMessage& received);
        virtual void handleMessage(const AddGfxEntityMessage& received);
        virtual void handleMessage(const MoveGfxEntityMessage& received);
        virtual void handleMessage(const RemoveGfxEntityMessage& received);
        void render();
        void cameraUpdate();    // camera function
		void setCameraMatrix(glm::mat4 m);
    private:
		ShaderProgram mShaderProgram;
		Camera cam;
		Timer mTimer;
		glm::mat4 projectionMatrix;
        fea::MessageBus& bus;
        std::vector<VBO> vbos;
        std::map<size_t, Billboard> billboards;
        GLuint blockTexture;
        glm::vec2 mScreenSize;
        // camera stuff
		float moveSpeed = 0.0001f;
		float lastY, lastX;
		bool movingLeft, movingRight, movingUp, movingDown, elevate, delevate;
		bool mouseDown = false;
		glm::vec3 camSpeed;
};
