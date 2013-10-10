#include "../messages.h"
#include "inputadaptor.h"
#include "inputactions.h"
#include <featherkit/util/input/sfml/sfmlinputbackend.h>

InputAdaptor::InputAdaptor(sf::Window& sfw, fea::MessageBus& b)
    :   inputHandler(new fea::util::SFMLInputBackend(sfw)),
        sfWindow(sfw),
        bus(b)
{
}

void InputAdaptor::update()
{
    fea::Event event;

    inputHandler.processEvents();

    while(inputHandler.pollEvent(event))
    {
        if(event.type == fea::Event::CLOSED)
        {
            bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::QUIT));
        }
        else if(event.type == fea::Event::KEYPRESSED)
        {
            if(event.key.code == fea::Keyboard::ESCAPE)
            {
                bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::QUIT));
            }
        }
    }
}
