#pragma once

class EntityController
{
    public:
        virtual void inspectEntity(fea::WeakEntityPtr entity) = 0;
        virtual void update() = 0;
    private:
        fea::EntityGroup entities;
};
