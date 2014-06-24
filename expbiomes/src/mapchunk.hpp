#pragma once
#include <glm/glm.hpp>
#include <array>
#include <fea/render2d.hpp>
#include "generator.hpp"
#include "biomestorage.hpp"

class MapChunk
{
    public:
        MapChunk();
        void setTexture(fea::Texture* tex);
        void setTexturePosition(const glm::vec2& pos);
        void generate(const glm::vec2& position, const glm::vec2& textureStart, const glm::vec2& textureEnd, int32_t amount);
        int32_t next;
        static Generator* generator;
        static BiomeStorage* storage;
        static std::array<glm::vec4, 5461>* queue;
        fea::Texture* texture;
        glm::vec2 texturePosition;
};
