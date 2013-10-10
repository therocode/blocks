#pragma once
#include "blockstd.h"
#include <featherkit/structure.h>
#include <featherkit/messaging.h>
#include <SFML/Window.hpp>
#include "world.h"
#include "camera.h"
#include "renderer.h"
#include "input/inputadaptor.h"

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
        fea::MessageBus bus;
        InputAdaptor inputAdaptor;
        World world;
        Renderer renderer;
		Camera cam;
		float moveSpeed = 0.001f;
		float lastY, lastX;
		bool movingLeft, movingRight, movingUp, movingDown, elevate, delevate;
		bool mouseDown = false;
		glm::vec3 camSpeed;
};
