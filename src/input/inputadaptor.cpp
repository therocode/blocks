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
            switch(event.key.code)
            {
				case fea::Keyboard::W:
                    bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::FORWARDS));
				break;
				case fea::Keyboard::S:
                    bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::BACKWARDS));
				break;
				case fea::Keyboard::A:
                    bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::LEFT));
				break;
				case fea::Keyboard::D:
                    bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::RIGHT));
				break;
				case fea::Keyboard::SPACE:
                    bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::JUMP));
				break;
				case fea::Keyboard::LSHIFT:
                    bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::CROUCH));
				break;

            }
        }else if(event.type == fea::Event::KEYRELEASED){
            switch(event.key.code)
            {
				case fea::Keyboard::W:
                    bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::STOPFORWARDS));
				break;
				case fea::Keyboard::S:
                    bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::STOPBACKWARDS));
				break;
				case fea::Keyboard::A:
                    bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::STOPLEFT));
				break;
				case fea::Keyboard::D:
                    bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::STOPRIGHT));
				break;
				case fea::Keyboard::SPACE:
                    bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::STOPJUMP));
				break;
				case fea::Keyboard::LSHIFT:
                    bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::STOPCROUCH));
				break;
            }
        }
    }
}
