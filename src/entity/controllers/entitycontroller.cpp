#include "entitycontroller.hpp"

EntityController::EntityController(fea::MessageBus& bus, GameInterface& worldInterface) : mBus(bus), mGameInterface(worldInterface)
{
}

void EntityController::onFrame(int dt)
{
}
