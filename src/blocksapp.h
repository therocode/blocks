#pragma once
#include <featherkit/structure.h>
#include <featherkit/userinterface.h>
#include <SFML/Window.hpp>
#include "world.h"

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
        World world;
};
