#pragma once
#include "controllers/physicstype.hpp"

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
