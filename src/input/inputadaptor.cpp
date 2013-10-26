#include "inputmessages.h"
#include "../script/scriptmessages.h"
#include "inputadaptor.h"
#include "inputactions.h"
#include <featherkit/util/input/sdl2/sdl2inputbackend.h>
#include <featherkit/userinterfaceutil.h>

	InputAdaptor::InputAdaptor(fea::MessageBus& b)
:   inputHandler(new fea::util::SDL2InputBackend),
    mPlayerId(-1),
	mBus(b)
{
	fea::util::JsonActionIOHandler<std::string> jsonHandler;
	jsonHandler.loadBindingsFile("data/bindings.json");
	actionHandler.setPrimaryBindings(jsonHandler.getPrimaryBindings());
	actionHandler.setSecondaryBindings(jsonHandler.getSecondaryBindings());

	lastMouseX = 0;
	lastMouseY = 0;
	first = true;
	mouseDown = false;
	windowFocus = true;
    mBus.addMessageSubscriber<PlayerIdMessage>(*this);   
}

InputAdaptor::~InputAdaptor()
{
    mBus.removeMessageSubscriber<PlayerIdMessage>(*this);   
}

void InputAdaptor::update()
{
	fea::Event event;
	std::string action;

	inputHandler.processEvents();
	actionHandler.processActions(inputHandler);

	while(inputHandler.pollEvent(event))
	{
		if(event.type == fea::Event::GAINEDFOCUS)
			windowFocus = true;
		else if(event.type == fea::Event::LOSTFOCUS)
			windowFocus = false;
		else
		if(event.type == fea::Event::CLOSED)
		{
			mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::QUIT));
		}
		else if(event.type == fea::Event::MOUSEMOVED){
			if(!first)
			{
				if(windowFocus)
				{
					float pitch = -event.mouseMove.rely;
					float yaw   = -event.mouseMove.relx;
					float sensitivity = 0.2f;
					pitch *= sensitivity;
					yaw   *= sensitivity;
					mBus.sendMessage<PlayerPitchYawMessage>(PlayerPitchYawMessage(mPlayerId, pitch, yaw));
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
			//    mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::MOUSELEFT));
			//}
		}
		else if(event.type == fea::Event::MOUSEBUTTONRELEASED){
			mouseDown = false;
			//if(event.mouseButton.button == fea::Mouse::Button::LEFT){
			//    mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::STOPMOUSELEFT));
			//}
		}
		else if(event.type == fea::Event::RESIZED){
			mBus.sendMessage<WindowResizeMessage>(WindowResizeMessage(event.size.width, event.size.height));
		}
	}

	while(actionHandler.pollAction(action))
	{
		if(action == "quit")
			mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::QUIT));
		else if(action == "forwards")
			mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::FORWARDS));
		else if(action == "backwards")
			mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::BACKWARDS));
		else if(action == "left")
			mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::LEFT));
		else if(action == "right")
			mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::RIGHT));
		else if(action == "jump")
			mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::JUMP));
		else if(action == "crouch")
			mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::CROUCH));
		else if(action == "rebuild_scripts")
			mBus.sendMessage<RebuildScriptsRequestedMessage>(RebuildScriptsRequestedMessage('0'));

		else if(action == "stopforwards")
			mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::STOPFORWARDS));
		else if(action == "stopbackwards")
			mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::STOPBACKWARDS));
		else if(action == "stopleft")
			mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::STOPLEFT));
		else if(action == "stopright")
			mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::STOPRIGHT));
		else if(action == "stopjump")
			mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::STOPJUMP));
		else if(action == "stopcrouch")
			mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::STOPCROUCH));
	}
}

void InputAdaptor::handleMessage(const PlayerIdMessage& received)
{
    std::tie(mPlayerId) = received.data;
}
