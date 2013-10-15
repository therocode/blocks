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

	//window.setFramerateLimit(30);
    renderer.setup();
    bus.sendMessage<WindowResizeMessage>(WindowResizeMessage(800, 600));

    world.initialise();
	window.lockCursor(true);
}

void BlocksApplication::loop()
{
    inputAdaptor.update();

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
