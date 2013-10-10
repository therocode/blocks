#include "blocksapp.h"
#include "messages.h"
#include "input/inputactions.h"
#include <featherkit/util/window/sfml/sfmlwindowbackend.h>
#include <featherkit/util/input/sfml/sfmlinputbackend.h>

BlocksApplication::BlocksApplication()
    :   window(new fea::util::SFMLWindowBackend(sfWindow)),
        inputAdaptor(sfWindow, bus),
        world(bus),
        renderer(bus)
{
	bus.addMessageSubscriber<InputActionMessage>(*this);
}

void BlocksApplication::setup()
{
    window.create(fea::VideoMode(800, 600, 32), "Blocky", fea::Style::Default, fea::ContextSettings(32));

    renderer.setup();
    bus.sendMessage<WindowResizeMessage>(WindowResizeMessage(800, 600));

    world.initialise();
}

void BlocksApplication::loop()
{
    inputAdaptor.update();
    /*
    fea::Event event;

    inputHandler.processEvents();

    while(inputHandler.pollEvent(event))
    {
        if(event.type == fea::Event::CLOSED)
            quit();
/////
		else if(event.type == fea::Event::MOUSEMOVED)
		{
			float newX = (float)event.mouseMove.x;
			float newY = (float)event.mouseMove.y;
			float mspeed = 0.01f;
			if(mouseDown)
				cam.AddDirection((newX - lastX)  * mspeed,(newY - lastY) * mspeed);
			lastX = event.mouseMove.x;
			lastY = event.mouseMove.y;
		}

		else if(event.type == fea::Event::MOUSEBUTTONPRESSED){
			if(event.mouseButton.button == fea::Mouse::Button::LEFT){
				mouseDown = true;
			}
		}
		else if(event.type == fea::Event::MOUSEBUTTONRELEASED){
			if(event.mouseButton.button == fea::Mouse::Button::LEFT){
				mouseDown = false;
			}
		}
        else if(event.type == fea::Event::KEYPRESSED)
        {
            if(event.key.code == fea::Keyboard::ESCAPE)
                quit();
            switch(event.key.code)
            {
				case fea::Keyboard::A:
					movingLeft = true;
				break;
				case fea::Keyboard::D:
					movingRight = true;
				break;
				case fea::Keyboard::W:
					movingUp = true;
				break;
				case fea::Keyboard::S:
					movingDown = true;
				break;
				case fea::Keyboard::SPACE:
					elevate = true;
				break;
				case fea::Keyboard::LSHIFT:
					delevate = true;
				break;

            }
        }else if(event.type == fea::Event::KEYRELEASED){
            switch(event.key.code)
            {
				case fea::Keyboard::A:
					movingLeft = false;
				break;
				case fea::Keyboard::D:
					movingRight = false;
				break;
				case fea::Keyboard::W:
					movingUp = false;
				break;
				case fea::Keyboard::S:
					movingDown = false;
				break;
				case fea::Keyboard::SPACE:
					elevate = false;
				break;
				case fea::Keyboard::LSHIFT:
					delevate = false;
				break;
            }
		}
        else if (event.type == fea::Event::RESIZED){
            bus.sendMessage<WindowResizeMessage>(WindowResizeMessage(event.size.width, event.size.height));
        }
    }
    */

    renderer.cameraUpdate();
    world.update();
    renderer.render();
    window.swapBuffers();
}

void BlocksApplication::destroy()
{
    window.close();
}

void BlocksApplication::handleMessage(const InputActionMessage& received)
{
    int action;
	std::tie(action) = received.data;

    if(action == InputAction::QUIT)
    {
        quit();
    }
}
