#include "entitycontroller.h"

EntityController::EntityController(fea::MessageBus& bus, WorldInterface& worldInterface) : mBus(bus), mWorldInterface(worldInterface)
{
}

void EntityController::onFrame()
{
}
