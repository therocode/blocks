#pragma once
#include "dimension.h"

class WorldInterface
{
    public:
        WorldInterface(Dimension& dim);

    private:
        Dimension& dimension;
};
