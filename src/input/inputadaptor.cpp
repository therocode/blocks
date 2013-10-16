#include "../messages.h"
#include "inputadaptor.h"
#include "inputactions.h"
#include <featherkit/util/input/sfml/sfmlinputbackend.h>
#include <featherkit/userinterfaceutil.h>

InputAdaptor::InputAdaptor(sf::Window& sfw, fea::MessageBus& b)
    :   inputHandler(new fea::util::SFMLInputBackend(sfw)),
        sfWindow(sfw),
        bus(b)
{
      fea::util::JsonActionIOHandler<std::string> jsonHandler;
      jsonHandler.loadBindingsFile("data/bindings.json");
      actionHandler.setPrimaryBindings(jsonHandler.getPrimaryBindings());
      actionHandler.setSecondaryBindings(jsonHandler.getSecondaryBindings());
}

void InputAdaptor::update()
{
    fea::Event event;
    std::string action;

    inputHandler.processEvents();
    actionHandler.processActions(inputHandler);

    while(inputHandler.pollEvent(event))
    {
        if(event.type == fea::Event::CLOSED)
        {
            bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::QUIT));
        }
		else if(event.type == fea::Event::MOUSEMOVED){
            bus.sendMessage<MouseMovedMessage>(MouseMovedMessage((float)event.mouseMove.x, (float)event.mouseMove.y));
		}
		else if(event.type == fea::Event::MOUSEBUTTONPRESSED){
			if(event.mouseButton.button == fea::Mouse::Button::LEFT){
                bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::MOUSELEFT));
			}
		}
		else if(event.type == fea::Event::MOUSEBUTTONRELEASED){
			if(event.mouseButton.button == fea::Mouse::Button::LEFT){
                bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::STOPMOUSELEFT));
			}
		}
        else if(event.type == fea::Event::RESIZED){
            bus.sendMessage<WindowResizeMessage>(WindowResizeMessage(event.size.width, event.size.height));
        }
    }
    
    while(actionHandler.pollAction(action))
    {
        if(action == "quit")
            bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::QUIT));
        else if(action == "forwards")
            bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::FORWARDS));
        else if(action == "backwards")
            bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::BACKWARDS));
        else if(action == "left")
            bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::LEFT));
        else if(action == "right")
            bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::RIGHT));
        else if(action == "jump")
            bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::JUMP));
        else if(action == "crouch")
            bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::CROUCH));
        else if(action == "rebuild_scripts")
            bus.sendMessage<RebuildScriptsRequestedMessage>(RebuildScriptsRequestedMessage());

        else if(action == "stopforwards")
            bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::STOPFORWARDS));
        else if(action == "stopbackwards")
            bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::STOPBACKWARDS));
        else if(action == "stopleft")
            bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::STOPLEFT));
        else if(action == "stopright")
            bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::STOPRIGHT));
        else if(action == "stopjump")
            bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::STOPJUMP));
        else if(action == "stopcrouch")
            bus.sendMessage<InputActionMessage>(InputActionMessage(InputAction::STOPCROUCH));
    }
}
