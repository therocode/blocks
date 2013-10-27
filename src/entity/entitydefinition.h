#pragma once
#include "controllers/physicstype.h"

struct EntityDefinition
{
    std::string name;
    std::string category;
    PhysicsType physicsType;
    float drag;
    float walkSpeed;
    float runSpeed;
    float jumpStrength;
};
