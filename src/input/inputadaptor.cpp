#include "inputmessages.hpp"
#include "../script/scriptmessages.hpp"
#include "../rendering/renderingmessages.hpp"
#include "inputadaptor.hpp"
#include "inputactions.hpp"
#include <fea/ui/sdl2inputbackend.hpp>
#include <fea/ui/jsonactioniohandler.hpp>
#include "../networking/networkingprotocol.hpp"

const std::string logName = "input";

InputAdaptor::InputAdaptor(fea::MessageBus& b):
    inputHandler(new fea::SDL2InputBackend),
    mBus(b),
    mHoldingForwards(false),
    mHoldingBackwards(false),
    mHoldingLeft(false),
    mHoldingRight(false),
    mMouseLocked(true)
{
    fea::JsonActionIOHandler<std::string> jsonHandler;
    jsonHandler.loadBindingsFile("data/bindings.json");
    actionHandler.setPrimaryBindings(jsonHandler.getPrimaryBindings());
    actionHandler.setSecondaryBindings(jsonHandler.getSecondaryBindings());

    inputHandler.setKeyRepeatEnabled(false);

    lastMouseX = 0;
    lastMouseY = 0;
    first = true;
    mouseDown = false;
    mNewPitch = mNewYaw = 0;
}

//void Client::handleMessage(const WindowFocusLostMessage& received){
//    mWindow.lockCursor(false);
//    mMouseLocked = false;
//}
//
//void Client::handleMessage(const WindowInputMessage& received){
//    if(!mMouseLocked){
//        mMouseLocked = true;
//    }
//}


void InputAdaptor::update()
{
    fea::Event event;
    std::string action;

    while(inputHandler.pollEvent(event))
    {
        actionHandler.feedEvent(event);

        if(event.type == fea::Event::GAINEDFOCUS)
        {
            mMouseLocked = true;
        }
        else if(event.type == fea::Event::LOSTFOCUS)
        {
            mMouseLocked = false;
            mMouseLocked = false;
            mBus.send(CursorLockedMessage{false});
        }
        else if(event.type == fea::Event::CLOSED)
        {
            mBus.send(ClientActionMessage{InputAction::QUIT});
        }
        else if(event.type == fea::Event::MOUSEMOVED)
        {
            if(!first)
            {
                if(mMouseLocked)
                {
                    float pitch = -event.mouseMove.rely;
                    float yaw   = -event.mouseMove.relx;
                    mNewYaw -= event.mouseMove.relx;
                    mNewPitch -= event.mouseMove.rely;
                    float sensitivity = 0.2f;
                    pitch *= sensitivity;
                    yaw   *= sensitivity;
                    ClientPitchYawMessage msg{pitch, yaw};
                   // msg.onlyLocal = true;
                    mBus.send(msg);
                   /* if(glm::abs(mNewYaw) > 10.f || glm::abs(mNewPitch) > 10.f)
                    {
                        mNewYaw *= sensitivity;
                        mNewPitch *= sensitivity;
                        msg.onlyLocal = false;
                        msg = PlayerPitchYawMessage(mPlayerId, mNewPitch, mNewYaw);
                        mBus.send(msg);
                        mNewPitch = mNewYaw = 0;
                    }*/
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
        else if(event.type == fea::Event::MOUSEBUTTONPRESSED)
        {
            mouseDown = true;
            //printf("clicked at psition %i, %i\n", event.mouseButton.x, event.mouseButton.y);
            if(event.mouseButton.button == fea::Mouse::Button::LEFT)
            {
                mBus.send(ClientActionMessage{InputAction::DIG});
            }
            else if(event.mouseButton.button == fea::Mouse::Button::RIGHT)
            {
                mBus.send(ClientActionMessage{InputAction::BUILD});
            }
            if(event.mouseButton.y > 1 && event.mouseButton.x > 10)
            {
                mBus.send(WindowInputMessage());
                mMouseLocked = true;
                mBus.send(CursorLockedMessage{true});
            }
        }
        else if(event.type == fea::Event::MOUSEWHEELMOVED)
        {
            if(event.mouseWheel.delta > 0)
            {
                mBus.send(ClientActionMessage{InputAction::DIG});
            }
            else if(event.mouseWheel.delta < 0)
            {
                mBus.send(ClientActionMessage{InputAction::BUILD});
            }
            if(event.mouseWheel.x > 0)
            {
                mBus.send(ClientPitchYawMessage{0, -1.0f});
            }else if(event.mouseWheel.x < 0)
            {
                mBus.send(ClientPitchYawMessage{0, 1.0f});
            }
        }
        else if(event.type == fea::Event::MOUSEBUTTONRELEASED)
        {
            mouseDown = false;
            if(event.mouseButton.button == fea::Mouse::Button::LEFT)
            {
                mBus.send(ClientActionMessage{InputAction::STOPMOUSELEFT});
            }
        }
        else if(event.type == fea::Event::RESIZED)
        {
            mBus.send(WindowResizeMessage{event.size.width, event.size.height});
        }
        else if(event.type == fea::Event::KEYPRESSED)
        {
            if(event.key.code == fea::Keyboard::NUM7)
                mBus.send(RenderModeMessage{DISABLE_ALL});
            else if(event.key.code == fea::Keyboard::NUM8)
                mBus.send(RenderModeMessage{DISABLE_DEBUG});
            else if(event.key.code == fea::Keyboard::NUM9)
                mBus.send(RenderModeMessage{DISABLE_MODEL});
            else if(event.key.code == fea::Keyboard::U)
                mBus.send(RenderModeMessage{ENABLE_ALL});
            else if(event.key.code == fea::Keyboard::I)
                mBus.send(RenderModeMessage{ENABLE_DEBUG});
            else if(event.key.code == fea::Keyboard::O)
                mBus.send(RenderModeMessage{ENABLE_MODEL});
            else if(event.key.code == fea::Keyboard::J)
                mBus.send(RenderModeMessage{TOGGLE_MODE_ALL});
            else if(event.key.code == fea::Keyboard::K)
                mBus.send(RenderModeMessage{TOGGLE_MODE_DEBUG});
            else if(event.key.code == fea::Keyboard::L)
                mBus.send(RenderModeMessage{TOGGLE_MODE_MODEL});
        }
    }

    while(actionHandler.pollAction(action))
    {
        if(action == "quit")
        {
            mBus.send(LogMessage{"Sending quit action.", logName, LogLevel::INFO});
            mBus.send(ClientActionMessage{InputAction::QUIT});
        }
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
            mBus.send(ClientActionMessage{InputAction::JUMP});
        else if(action == "crouch")
            mBus.send(ClientActionMessage{InputAction::CROUCH});
        else if(action == "rebuild_scripts")
            mBus.send(RebuildScriptsRequestedMessage());

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
            mBus.send(ClientActionMessage{InputAction::STOPJUMP});
        else if(action == "stopcrouch")
            mBus.send(ClientActionMessage{InputAction::STOPCROUCH});
        else if(action == "warp")
            mBus.send(ClientActionMessage{InputAction::WARP});
    }
}

void InputAdaptor::sendMovementData()
{
    direction.setDirection(mHoldingForwards, mHoldingBackwards, mHoldingLeft, mHoldingRight);

    mBus.send(ClientMoveDirectionMessage{direction});

    if(direction.isStill())
        mBus.send(ClientMoveActionMessage{MoveAction::STANDING});
    else
        mBus.send(ClientMoveActionMessage{MoveAction::WALKING});
}
