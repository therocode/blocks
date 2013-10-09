#pragma once
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
        void makeTexture(std::string path, uint32_t width, uint32_t height, GLuint& textureId);
        void setup();
        virtual void handleMessage(const ChunkCreatedMessage& received);
        virtual void handleMessage(const WindowResizeMessage& received);
        void render();
		void setCameraMatrix(glm::mat4 m);
    private:
        fea::MessageBus& bus;
        std::vector<ChunkVBO> chunks;
        GLuint blockTexture;
};
