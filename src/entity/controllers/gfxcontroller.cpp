#include "gfxcontroller.h"
#include "../../rendering/renderingmessages.h"
#include <iostream>

GfxController::GfxController(fea::MessageBus& bus, WorldInterface& worldInterface) : EntityController(bus, worldInterface)
{
    bus.addMessageSubscriber<EntityMovedMessage>(*this);
}

GfxController::~GfxController()
{
    bus.removeMessageSubscriber<EntityMovedMessage>(*this);
}

void GfxController::inspectEntity(fea::WeakEntityPtr entity)
{
    fea::EntityPtr locked = entity.lock();

    if(locked->hasAttribute("position"))
    {
        mEntities.emplace(locked->getId(), entity);
        mBus.sendMessage<AddGfxEntityMessage>(AddGfxEntityMessage(locked->getId(), locked->getAttribute<glm::vec3>("position")));
    }
}

void GfxController::handleMessage(const EntityMovedMessage& message)
{
   size_t id;
   glm::vec3 oldPosition;
   glm::vec3 newPosition;
   
   std::tie(id, oldPosition, newPosition) = message.data;
   
   mBus.sendMessage<MoveGfxEntityMessage>(MoveGfxEntityMessage(id, newPosition));
}

void GfxController::removeEntity(fea::EntityId id)
{
    mBus.sendMessage<RemoveGfxEntityMessage>(RemoveGfxEntityMessage(id));
}

