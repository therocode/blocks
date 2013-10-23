#include "inputmessages.h"
#include "../script/scriptmessages.h"
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

      lastMouseX = 0;
      lastMouseY = 0;
      first = true;
      mouseDown = false;
}

void InputAdaptor::update()
{
    size_t playerId = 0; //this needs changing

    fea::Event event;
    std::string action;

    inputHandler.processEvents();
    actionHandler.processActions(inputHandler);

    while(inputHandler.pollEvent(event))
    {
        if(event.type == fea::Event::CLOSED)
        {
            bus.sendMessage<PlayerActionMessage>(PlayerActionMessage(playerId, InputAction::QUIT));
        }
		else if(event.type == fea::Event::MOUSEMOVED){
            if(!first)
            {
                if(mouseDown)
                {
                    bus.sendMessage<PlayerPitchYawMessage>(PlayerPitchYawMessage(playerId, (float)(event.mouseMove.y - lastMouseY), (float)(event.mouseMove.x - lastMouseX)));
                }
                lastMouseX = event.mouseMove.x;
                lastMouseY = event.mouseMove.y;
            }
            else
            {
                first = false;

                lastMouseX = event.mouseMove.x;
                lastMouseY = event.mouseMove.y;
            }
		}
		else if(event.type == fea::Event::MOUSEBUTTONPRESSED){
            mouseDown = true;
			//if(event.mouseButton.button == fea::Mouse::Button::LEFT){
            //    bus.sendMessage<PlayerActionMessage>(PlayerActionMessage(playerId, InputAction::MOUSELEFT));
			//}
		}
		else if(event.type == fea::Event::MOUSEBUTTONRELEASED){
            mouseDown = false;
			//if(event.mouseButton.button == fea::Mouse::Button::LEFT){
            //    bus.sendMessage<PlayerActionMessage>(PlayerActionMessage(playerId, InputAction::STOPMOUSELEFT));
			//}
		}
        else if(event.type == fea::Event::RESIZED){
            bus.sendMessage<WindowResizeMessage>(WindowResizeMessage(event.size.width, event.size.height));
        }
    }
    
    while(actionHandler.pollAction(action))
    {
        if(action == "quit")
            bus.sendMessage<PlayerActionMessage>(PlayerActionMessage(playerId, InputAction::QUIT));
        else if(action == "forwards")
            bus.sendMessage<PlayerActionMessage>(PlayerActionMessage(playerId, InputAction::FORWARDS));
        else if(action == "backwards")
            bus.sendMessage<PlayerActionMessage>(PlayerActionMessage(playerId, InputAction::BACKWARDS));
        else if(action == "left")
            bus.sendMessage<PlayerActionMessage>(PlayerActionMessage(playerId, InputAction::LEFT));
        else if(action == "right")
            bus.sendMessage<PlayerActionMessage>(PlayerActionMessage(playerId, InputAction::RIGHT));
        else if(action == "jump")
            bus.sendMessage<PlayerActionMessage>(PlayerActionMessage(playerId, InputAction::JUMP));
        else if(action == "crouch")
            bus.sendMessage<PlayerActionMessage>(PlayerActionMessage(playerId, InputAction::CROUCH));
        else if(action == "rebuild_scripts")
            bus.sendMessage<RebuildScriptsRequestedMessage>(RebuildScriptsRequestedMessage('0'));

        else if(action == "stopforwards")
            bus.sendMessage<PlayerActionMessage>(PlayerActionMessage(playerId, InputAction::STOPFORWARDS));
        else if(action == "stopbackwards")
            bus.sendMessage<PlayerActionMessage>(PlayerActionMessage(playerId, InputAction::STOPBACKWARDS));
        else if(action == "stopleft")
            bus.sendMessage<PlayerActionMessage>(PlayerActionMessage(playerId, InputAction::STOPLEFT));
        else if(action == "stopright")
            bus.sendMessage<PlayerActionMessage>(PlayerActionMessage(playerId, InputAction::STOPRIGHT));
        else if(action == "stopjump")
            bus.sendMessage<PlayerActionMessage>(PlayerActionMessage(playerId, InputAction::STOPJUMP));
        else if(action == "stopcrouch")
            bus.sendMessage<PlayerActionMessage>(PlayerActionMessage(playerId, InputAction::STOPCROUCH));
    }
}
