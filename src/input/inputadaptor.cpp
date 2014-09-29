#include "inputmessages.hpp"
#include "../script/scriptmessages.hpp"
#include "inputadaptor.hpp"
#include "inputactions.hpp"
#include <fea/ui/sdl2inputbackend.hpp>
#include <fea/ui/jsonactioniohandler.hpp>
#include "zlib.h"

const std::string logName = "input";

InputAdaptor::InputAdaptor(fea::MessageBus& b):
    inputHandler(new fea::SDL2InputBackend),
    mPlayerId(-1),
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
    subscribe(mBus, *this);
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
            mBus.send(PlayerActionMessage{mPlayerId, InputAction::QUIT});
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
                    PlayerPitchYawMessage msg{mPlayerId, pitch, yaw};
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
                mBus.send(PlayerActionMessage{mPlayerId, InputAction::DIG});
            }
            else if(event.mouseButton.button == fea::Mouse::Button::RIGHT)
            {
                mBus.send(PlayerActionMessage{mPlayerId, InputAction::BUILD});
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
                mBus.send(PlayerActionMessage{mPlayerId, InputAction::DIG});
            }
            else if(event.mouseWheel.delta < 0)
            {
                mBus.send(PlayerActionMessage{mPlayerId, InputAction::BUILD});
            }
            if(event.mouseWheel.x > 0)
            {
                mBus.send(PlayerPitchYawMessage{mPlayerId, 0, -1.0f});
            }else if(event.mouseWheel.x < 0)
            {
                mBus.send(PlayerPitchYawMessage{mPlayerId, 0, 1.0f});
            }
        }
        else if(event.type == fea::Event::MOUSEBUTTONRELEASED)
        {
            mouseDown = false;
            if(event.mouseButton.button == fea::Mouse::Button::LEFT)
            {
                mBus.send(PlayerActionMessage{mPlayerId, InputAction::STOPMOUSELEFT});
            }
        }
        else if(event.type == fea::Event::RESIZED)
        {
            mBus.send(WindowResizeMessage{event.size.width, event.size.height});
        }
    }

    while(actionHandler.pollAction(action))
    {
        if(action == "quit")
        {
            mBus.send(LogMessage{"Sending quit action.", logName, LogLevel::INFO});
            mBus.send(PlayerActionMessage{mPlayerId, InputAction::QUIT});
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
            mBus.send(PlayerActionMessage{mPlayerId, InputAction::JUMP});
        else if(action == "crouch")
            mBus.send(PlayerActionMessage{mPlayerId, InputAction::CROUCH});
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
            mBus.send(PlayerActionMessage{mPlayerId, InputAction::STOPJUMP});
        else if(action == "stopcrouch")
            mBus.send(PlayerActionMessage{mPlayerId, InputAction::STOPCROUCH});
        else if(action == "warp")
            mBus.send(PlayerActionMessage{mPlayerId, InputAction::WARP});
    }
}

void InputAdaptor::handleMessage(const PlayerIdMessage& received)
{
    mPlayerId = received.id;
}

void InputAdaptor::sendMovementData()
{
    direction.setDirection(mHoldingForwards, mHoldingBackwards, mHoldingLeft, mHoldingRight);

    mBus.send(PlayerMoveDirectionMessage{mPlayerId, direction});

    if(direction.isStill())
        mBus.send(PlayerMoveActionMessage{mPlayerId, MoveAction::STANDING});
    else
        mBus.send(PlayerMoveActionMessage{mPlayerId, MoveAction::WALKING});
}
