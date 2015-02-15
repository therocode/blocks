#include "gfxcontroller.hpp"
#include "../../rendering/renderingmessages.hpp"

GfxController::GfxController(fea::MessageBus& bus) : EntityController(bus)
{
    subscribe(mBus, *this);
}

bool GfxController::keepEntity(fea::WeakEntityPtr entity) const
{
    fea::EntityPtr locked = entity.lock();
    return locked->hasAttribute("position");
}

void GfxController::entityKept(fea::WeakEntityPtr entity)
{
    //fea::EntityPtr locked = entity.lock();
    //size_t returnedId;
    //mBus.send(AddGfxEntityMessage{locked->getAttribute<glm::vec3>("position"), returnedId});
    //std::cout << "returned Id: " << returnedId << "\n";
}

void GfxController::handleMessage(const EntityMovedMessage& message)
{
   mBus.send(MoveGfxEntityMessage{message.entityId, message.newPosition});
}

void GfxController::entityDropped(fea::WeakEntityPtr entity)
{
    mBus.send(RemoveGfxEntityMessage{entity.lock()->getId()});
}

