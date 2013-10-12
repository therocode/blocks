#pragma once
#include <vector>
#include <memory>
#include "controllers/entitycontroller.h"

class EntitySystem
{
    public:
        EntitySystem(fea::MessageBus& b);
        void initialise();
        void update();
        fea::WeakEntityPtr spawnEntity(const std::string& type);
        void attachEntity(fea::WeakEntityPtr entity);
    private:
        fea::MessageBus& bus;
        fea::EntityManager manager;
        std::vector<std::unique_ptr<EntityController> > controllers;
};
