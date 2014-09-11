#include "entitycontroller.hpp"

EntityController::EntityController(fea::MessageBus& bus, WorldInterface& worldInterface) : mBus(bus), mWorldInterface(worldInterface)
{
}

void EntityController::onFrame(int dt)
{
}
