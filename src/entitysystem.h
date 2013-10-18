#pragma once
#include <vector>
#include <memory>
#include <featherkit/entitysystem.h>
#include "controllers/entitycontroller.h"

class EntitySystem
{
    public:
        EntitySystem(fea::MessageBus& b);
        void initialise();
        void addController(std::unique_ptr<EntityController> controller);
        void update();
        fea::WeakEntityPtr spawnEntity(const std::string& type, const std::string& scriptType);
        void attachEntity(fea::WeakEntityPtr entity);
    private:
        fea::MessageBus& bus;
        fea::EntityManager manager;
        std::vector<std::unique_ptr<EntityController> > controllers;
};
