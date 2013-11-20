#include "inputmessages.h"
#include "../script/scriptmessages.h"
#include "inputadaptor.h"
#include "inputactions.h"
#include <featherkit/util/input/sdl2/sdl2inputbackend.h>
#include <featherkit/userinterfaceutil.h>

	InputAdaptor::InputAdaptor(fea::MessageBus& b)
:   inputHandler(new fea::util::SDL2InputBackend),
    mPlayerId(-1),
	mBus(b),
    mHoldingForwards(false),
    mHoldingBackwards(false),
    mHoldingLeft(false),
    mHoldingRight(false)
{
	fea::util::JsonActionIOHandler<std::string> jsonHandler;
	jsonHandler.loadBindingsFile("data/bindings.json");
	actionHandler.setPrimaryBindings(jsonHandler.getPrimaryBindings());
	actionHandler.setSecondaryBindings(jsonHandler.getSecondaryBindings());

    inputHandler.setKeyRepeatEnabled(false);

	lastMouseX = 0;
	lastMouseY = 0;
	first = true;
	mouseDown = false;
	windowFocus = true;
    mBus.addMessageSubscriber<PlayerIdMessage>(*this);
    mNewPitch = mNewYaw = 0;
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
		if(event.type == fea::Event::GAINEDFOCUS){
			windowFocus = true;
        }
		else if(event.type == fea::Event::LOSTFOCUS){
			windowFocus = false;
            mBus.sendMessage<WindowFocusLostMessage>(WindowFocusLostMessage());
        }
		else if(event.type == fea::Event::CLOSED)
		{
			mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::QUIT));
		}
		else if(event.type == fea::Event::MOUSEMOVED){
			if(!first)
			{
				if(windowFocus)
				{
                    mNewYaw -= event.mouseMove.relx;
                    mNewPitch -= event.mouseMove.rely;
                    if(glm::abs(mNewYaw) > 10.f || glm::abs(mNewPitch) > 10.f){
					float pitch = mNewPitch;//-event.mouseMove.rely;
					float yaw   = mNewYaw;//-event.mouseMove.relx;
                    mNewPitch = mNewYaw = 0;
					float sensitivity = 0.2f;
					pitch *= sensitivity;
					yaw   *= sensitivity;
					mBus.sendMessage<PlayerPitchYawMessage>(PlayerPitchYawMessage(mPlayerId, pitch, yaw));
                    }
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
            //printf("clicked at psition %i, %i\n", event.mouseButton.x, event.mouseButton.y);
			if(event.mouseButton.button == fea::Mouse::Button::LEFT){
			    mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::DIG));
			}
			else if(event.mouseButton.button == fea::Mouse::Button::RIGHT){
			    mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::BUILD));
			}
            if(event.mouseButton.y > 1 && event.mouseButton.x > 10){
                mBus.sendMessage<WindowInputMessage>(WindowInputMessage());
            }
		}else if(event.type == fea::Event::MOUSEWHEELMOVED){
            if(event.mouseWheel.delta > 0){
                mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::DIG));
            }else if(event.mouseWheel.delta < 0){
                mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::BUILD));
            }
            if(event.mouseWheel.x > 0){
				mBus.sendMessage<PlayerPitchYawMessage>(PlayerPitchYawMessage(mPlayerId, 0, -1.0f));
            }else if(event.mouseWheel.x < 0){
				mBus.sendMessage<PlayerPitchYawMessage>(PlayerPitchYawMessage(mPlayerId, 0, 1.0f));
            }
        }
		else if(event.type == fea::Event::MOUSEBUTTONRELEASED){
			mouseDown = false;
			if(event.mouseButton.button == fea::Mouse::Button::LEFT){
			    mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::STOPMOUSELEFT));
			}
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
        {
            mHoldingForwards = true;
            sendMovementData();
        }
		else if(action == "backwards")
        {
            mHoldingBackwards = true;
            sendMovementData();
        }
		else if(action == "left")
        {
            mHoldingLeft = true;
            sendMovementData();
        }
		else if(action == "right")
        {
            mHoldingRight = true;
            sendMovementData();
        }
		else if(action == "jump")
			mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::JUMP));
		else if(action == "crouch")
			mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(mPlayerId, InputAction::CROUCH));
		else if(action == "rebuild_scripts")
			mBus.sendMessage<RebuildScriptsRequestedMessage>(RebuildScriptsRequestedMessage('0'));

		else if(action == "stopforwards")
        {
            mHoldingForwards = false;
            sendMovementData();
        }
		else if(action == "stopbackwards")
        {
            mHoldingBackwards = false;
            sendMovementData();
        }
		else if(action == "stopleft")
        {
            mHoldingLeft = false;
            sendMovementData();
        }
		else if(action == "stopright")
        {
            mHoldingRight = false;
            sendMovementData();
        }
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

void InputAdaptor::sendMovementData()
{
    direction.setDirection(mHoldingForwards, mHoldingBackwards, mHoldingLeft, mHoldingRight);

    mBus.sendMessage<PlayerMoveDirectionMessage>(PlayerMoveDirectionMessage(mPlayerId, direction));

    if(direction.isStill())
        mBus.sendMessage<PlayerMoveActionMessage>(PlayerMoveActionMessage(mPlayerId, MoveAction::STANDING));
    else
        mBus.sendMessage<PlayerMoveActionMessage>(PlayerMoveActionMessage(mPlayerId, MoveAction::WALKING));
}
