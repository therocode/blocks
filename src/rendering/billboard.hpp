#pragma once
#include "vbo.hpp"

struct Billboard
{
    Billboard(const VBO& vbo, const glm::vec3& position, float pitch = 0.0f, float yaw = 0.0f);
    VBO mVbo;
    glm::vec3 mPosition;
    float mPitch;
    float mYaw;
};
