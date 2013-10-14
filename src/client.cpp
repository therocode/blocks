#include "client.h"
#include <iostream>
#include <featherkit/util/window/sfml/sfmlwindowbackend.h>
#include <featherkit/util/input/sfml/sfmlinputbackend.h>

Client::Client() : window(new fea::util::SFMLWindowBackend(sfWindow)),
                   renderer(mBus),
                   inputAdaptor(sfWindow, mBus),
                   quit(false),
                   mBridge(nullptr)
{
	mBus.addMessageSubscriber<InputActionMessage>(*this);
}

void Client::setup()
{
    window.create(fea::VideoMode(800, 600, 32), "Blocky", fea::Style::Default, fea::ContextSettings(32));

	//window.setFramerateLimit(30);
    renderer.setup();
    mBus.sendMessage<WindowResizeMessage>(WindowResizeMessage(800, 600));

    std::cout << "client setup\n";
}

void Client::handleInput()
{
    //fetch server stuff

    inputAdaptor.update();

    mBridge->flush();
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
    std::cout << "client destroyed\n";
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

void Client::setServerBridge(std::unique_ptr<ServerClientBridge> bridge)
{
    mBridge = std::move(bridge);
    std::cout << "client connected to server\n";
}
