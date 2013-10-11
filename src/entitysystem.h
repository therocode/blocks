#pragma once

class EntitySystem
{
    public:
        void initialise();
        void update();
    private:
        std::vector<unique_ptr<EntityController> > controllers;
};
