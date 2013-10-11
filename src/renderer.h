#pragma once
#include "blockstd.h"
#include "chunk.h"
#include "chunkvbo.h"
#include "messages.h"
#include "camera.h"
#include <vector>
#include <featherkit/messaging.h>
#include "timer.h"

class Renderer
    :   public fea::MessageReceiver<ChunkCreatedMessage>,
        public fea::MessageReceiver<WindowResizeMessage>,
        public fea::MessageReceiver<InputActionMessage>,
        public fea::MessageReceiver<MouseMovedMessage>
                
{
    public:
        Renderer(fea::MessageBus& messageBus);
        ~Renderer();
		Camera cam;
        void makeTexture(std::string path, uint32_t width, uint32_t height, GLuint& textureId);
        void setup();
        virtual void handleMessage(const ChunkCreatedMessage& received);
        virtual void handleMessage(const WindowResizeMessage& received);
        virtual void handleMessage(const InputActionMessage& received);
        virtual void handleMessage(const MouseMovedMessage& received);
        void render();
        void cameraUpdate();    // camera function
		void setCameraMatrix(glm::mat4 m);
    private:

		Timer mTimer;
		glm::mat4 projectionMatrix;
        fea::MessageBus& bus;
        std::vector<ChunkVBO> chunks;
        GLuint blockTexture;
        
        // camera stuff
		float moveSpeed = 0.0001f;
		float lastY, lastX;
		bool movingLeft, movingRight, movingUp, movingDown, elevate, delevate;
		bool mouseDown = false;
		glm::vec3 camSpeed;
};
