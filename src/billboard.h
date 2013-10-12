#pragma once
#include "vbo.h"

struct Billboard
{
    Billboard(const VBO& vbo, const glm::vec3& position);
    VBO mVbo;
    glm::vec3 mPosition;
};
