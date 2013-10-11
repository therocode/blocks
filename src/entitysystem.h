#pragma once
#include <vector>
#include <memory>
#include "controllers/entitycontroller.h"

class EntitySystem
{
    public:
        void initialise();
        void update();
    private:
        std::vector<std::unique_ptr<EntityController> > controllers;
};
