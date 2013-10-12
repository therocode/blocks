#include "gfxcontroller.h"
#include <iostream>

GfxController::GfxController(fea::MessageBus& b) : EntityController(b)
{
    bus.addMessageSubscriber<EntityMovedMessage>(*this);
}

void GfxController::inspectEntity(fea::WeakEntityPtr entity)
{
    fea::EntityPtr locked = entity.lock();

    if(locked->hasAttribute("position"))
    {
        entities.emplace(locked->getId(), entity);
        bus.sendMessage<AddGfxEntityMessage>(AddGfxEntityMessage(locked->getId(), locked->getAttribute<glm::vec3>("position")));
        std::cout << "spawned the entity at " << locked->getAttribute<glm::vec3>("position").x << " " << locked->getAttribute<glm::vec3>("position").y << " " << locked->getAttribute<glm::vec3>("position").z << "\n";
    }
}

void GfxController::handleMessage(const EntityMovedMessage& message)
{
   size_t id;
   glm::vec3 oldPosition;
   glm::vec3 newPosition;
   
   std::tie(id, oldPosition, newPosition) = message.data;
   
   bus.sendMessage<MoveGfxEntityMessage>(MoveGfxEntityMessage(id, newPosition));

   std::cout << "moved the entity to " << newPosition.x << " " << newPosition.y << " " << newPosition.z << "\n";
}
