#pragma once
#include "chunk.h"
#include "chunkvbo.h"
#include "messages.h"
#include <vector>
#include <featherkit/messaging.h>

class Renderer : fea::MessageReceiver<ChunkCreatedMessage>
{
    public:
        Renderer(fea::MessageBus& messageBus);
        ~Renderer();
        void setup();
        virtual void handleMessage(const ChunkCreatedMessage& received);
        void render();
        void hehehe();
		void setCameraMatrix(glm::mat4 m);
    private:
        fea::MessageBus& bus;
        std::vector<ChunkVBO> chunks;
        float hehe = 0.0f;
};
