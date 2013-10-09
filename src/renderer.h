#pragma once
#include "blockstd.h"
#include "chunk.h"
#include "chunkvbo.h"
#include "messages.h"
#include <vector>
#include <featherkit/messaging.h>

class Renderer
    :   public fea::MessageReceiver<ChunkCreatedMessage>,
        public fea::MessageReceiver<WindowResizeMessage>
                
{
    public:
        Renderer(fea::MessageBus& messageBus);
        ~Renderer();
        void setup();
        virtual void handleMessage(const ChunkCreatedMessage& received);
        virtual void handleMessage(const WindowResizeMessage& received);
        void render();
        void hehehe();
		void setCameraMatrix(glm::mat4 m);
    private:
		glm::mat4 projectionMatrix;
        fea::MessageBus& bus;
        std::vector<ChunkVBO> chunks;
        float hehe = 0.0f;
};
