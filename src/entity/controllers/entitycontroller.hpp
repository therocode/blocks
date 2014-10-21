#pragma once
#include <fea/util.hpp>
#include <fea/entitysystem.hpp>

class GameInterface;

class EntityController : public fea::EntityComponent
{
    public:
        EntityController(fea::MessageBus& bus);
    protected:
        fea::MessageBus& mBus;
};
