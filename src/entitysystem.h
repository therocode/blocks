#pragma once
#include <vector>
#include <memory>
#include "controllers/entitycontroller.h"

class EntitySystem
{
    public:
        void initialise();
        void update();
        fea::WeakEntityPtr spawnEntity(const std::string& type);
    private:
        fea::EntityManager manager;
        std::vector<std::unique_ptr<EntityController> > controllers;
};
