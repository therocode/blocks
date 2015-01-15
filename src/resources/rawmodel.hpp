#pragma once
#include <vector>
#include "../utilities/glm.hpp"

struct FrameTranslation
{
    glm::mat3x3 rotation;
    glm::vec3 translation;
};

struct RawModel
{
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<std::vector<uint32_t>> indices;
    std::vector<uint8_t> blendIndices;
    std::vector<uint8_t> blendWeights;
    std::vector<FrameTranslation> frames;
};
