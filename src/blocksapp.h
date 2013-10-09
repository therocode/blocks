#pragma once
#include "blockstd.h"
#include <featherkit/structure.h>
#include <featherkit/userinterface.h>
#include <featherkit/messaging.h>
#include <SFML/Window.hpp>
#include "world.h"
#include "camera.h"
class BlocksApplication : public fea::Application
{
    public:
        BlocksApplication();
        void setup() override;
        void loop() override;
        void destroy() override;
    private:

        sf::Window sfWindow;
        fea::Window window;
        fea::InputHandler inputHandler;
        fea::MessageBus bus;
        World world;
        Renderer renderer;
		Camera cam;
		float moveSpeed = .00001f;
		float lastY, lastX;
		bool movingLeft, movingRight, movingUp, movingDown;
		glm::vec3 camSpeed;
};
