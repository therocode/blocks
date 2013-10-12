#pragma once
#include "dimension.h"
#include "entitysystem.h"

class WorldInterface
{
    public:
        WorldInterface(Dimension& dimension, EntitySystem& entitySystem);

    private:
        Dimension& mDimension;
        EntitySystem& mEntitySystem;
};
