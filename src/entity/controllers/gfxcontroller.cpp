#include "gfxcontroller.h"
#include "../../rendering/renderingmessages.h"
#include <iostream>

GfxController::GfxController(fea::MessageBus& bus, WorldInterface& worldInterface) : EntityController(bus, worldInterface)
{
    mBus.addSubscriber<EntityMovedMessage>(*this);
}

GfxController::~GfxController()
{
    mBus.removeSubscriber<EntityMovedMessage>(*this);
}

void GfxController::inspectEntity(fea::WeakEntityPtr entity)
{
    fea::EntityPtr locked = entity.lock();

    if(locked->hasAttribute("position"))
    {
        mEntities.emplace(locked->getId(), entity);
        mBus.send(AddGfxEntityMessage{locked->getId(), locked->getAttribute<glm::vec3>("position")});
    }
}

void GfxController::handleMessage(const EntityMovedMessage& message)
{
   mBus.send(MoveGfxEntityMessage{message.id, message.newPosition});
}

void GfxController::removeEntity(fea::EntityId id)
{
    mBus.send(RemoveGfxEntityMessage{id});
}

