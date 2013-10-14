#include "client.h"
#include <featherkit/util/window/sfml/sfmlwindowbackend.h>
#include <featherkit/util/input/sfml/sfmlinputbackend.h>

Client::Client() : window(new fea::util::SFMLWindowBackend(sfWindow)),
                   renderer(mBus),
                   inputAdaptor(sfWindow, mBus),
                   quit(false)
{
	mBus.addMessageSubscriber<InputActionMessage>(*this);
}

void Client::setup()
{
    window.create(fea::VideoMode(800, 600, 32), "Blocky", fea::Style::Default, fea::ContextSettings(32));

	//window.setFramerateLimit(30);
    renderer.setup();
    mBus.sendMessage<WindowResizeMessage>(WindowResizeMessage(800, 600));
}

void Client::handleInput()
{
    inputAdaptor.update();
}

void Client::render()
{
    renderer.cameraUpdate();
    renderer.render();
    window.swapBuffers();
}

void Client::destroy()
{
    window.close();
}

void Client::handleMessage(const InputActionMessage& received)
{
    int action;
	std::tie(action) = received.data;

    if(action == InputAction::QUIT)
    {
        quit = true;
    }
}

bool Client::requestedQuit()
{
    return quit;
}
