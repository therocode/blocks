#include "blocksapp.h"
#include <featherkit/util/window/sfml/sfmlwindowbackend.h>
#include <featherkit/util/input/sfml/sfmlinputbackend.h>

BlocksApplication::BlocksApplication() : window(new fea::util::SFMLWindowBackend(sfWindow)), inputHandler(new fea::util::SFMLInputBackend(sfWindow))
{
}

void BlocksApplication::setup()
{
    window.create(fea::VideoMode(800, 600, 32), "Blocky");
}

void BlocksApplication::loop()
{
    fea::Event event;

    inputHandler.processEvents();
    while(inputHandler.pollEvent(event))
    {
        if(event.type == fea::Event::CLOSED)
            quit();

        if(event.type == fea::Event::KEYPRESSED)
        {
            if(event.key.code == fea::Keyboard::ESCAPE)
                quit();
        }
    }
}

void BlocksApplication::destroy()
{
    window.close();
}
