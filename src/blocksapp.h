#pragma once
#include "blockstd.h"
#include <featherkit/structure.h>
#include <featherkit/messaging.h>
#include <SFML/Window.hpp>
#include "world.h"
#include "renderer.h"
#include "input/inputadaptor.h"

class BlocksApplication 
    :   public fea::Application,
        public fea::MessageReceiver<InputActionMessage>
{
    public:
        BlocksApplication();
        void setup() override;
        void loop() override;
        void destroy() override;
        virtual void handleMessage(const InputActionMessage& received);

    private:
        sf::Window sfWindow;
        fea::Window window;
        fea::MessageBus bus;
        InputAdaptor inputAdaptor;
        World world;
        Renderer renderer;
};
