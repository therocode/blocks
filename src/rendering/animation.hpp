#pragma once
#include <vector>
#include "../utilities/glm.hpp"

struct Animation
{
    std::vector<glm::mat3x3> rotations;
    std::vector<glm::vec3> translations;
    float framerate;
};
