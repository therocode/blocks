#pragma once
#include <vector>
#include "../utilities/glm.hpp"

struct RawAnimation
{
    std::string name;
    std::vector<glm::mat3x3> rotations;
    std::vector<glm::vec3> translations;
    float framerate;
};

struct RawModel
{
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<std::vector<uint32_t>> indices;
    std::vector<uint8_t> blendIndices;
    std::vector<uint8_t> blendWeights;
    std::vector<RawAnimation> animations;
};
