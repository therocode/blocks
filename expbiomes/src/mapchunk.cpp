#include "mapchunk.hpp"
#include <iostream>

MapChunk::MapChunk() :
    next(0)
{
}

void MapChunk::setTexture(fea::Texture* tex, fea::Texture* dataText)
{
    texture = tex;
    dataTexture = dataText;
}

void MapChunk::setTexturePosition(const glm::vec2& pos)
{
    texturePosition = pos;
}

void MapChunk::setQuad(fea::Quad* q)
{
    quad = q;
}
        
fea::Quad* MapChunk::getQuad()
{
    return quad;
}

void MapChunk::generate(const glm::vec2& position, const glm::vec2& textureStart, const glm::vec2& textureEnd, int32_t amount)
{
    if(next < 5460)
    {
        for(int32_t i = 0; i < amount; i++)
        {
            if(next == 5460)
                break;

            glm::vec4 region = (*queue)[next++];
            glm::vec2 perc = glm::vec2(region.x + (region.z - region.x) / 2.0f, region.y + (region.w - region.y) / 2.0f);
            glm::vec2 midpoint = position + 64.0f * perc;

            //std::cout << "HEJHEJ\n";
            //std::cout << "position: " << position.x << " " << position.y << "\n";
            //std::cout << "region start: " << region.x << " " << region.y << "\n";
            //std::cout << "region end: " << region.z << " " << region.w << "\n";
            //std::cout << "midpoint: " << midpoint.x << " " << midpoint.y << "\n";

            float height, rain, temperature, biome;

            generator->generateCoordinate(midpoint.x, midpoint.y, height, rain, temperature, biome);
            Biome* biomePtr = storage->getBiome(temperature, rain, height, biome);

            fea::Color col;

            if(biomePtr != nullptr)
            {
                float h = std::max(0.7f, height);
                col = fea::Color(biomePtr->r * h, biomePtr->g * h, biomePtr->b * h);
            }

            glm::ivec2 start = (glm::ivec2)(texturePosition + (glm::vec2(region.x, region.y) * 64.0f));
            glm::ivec2 end   = (glm::ivec2)(texturePosition + (glm::vec2(region.z, region.w) * 64.0f));
            //std::cout << "start: " << start.x << " " << start.y << "\n";
            //std::cout << "end: " << end.x << " " << end.y << "\n";
            int32_t realX, realY = 0;
            for(int32_t y = start.y; y < end.y; y++)
            {
                for(int32_t x = start.x; x < end.x; x++)
                {
                    int32_t index = realX + realY * 64;
                    texture->setPixel(x, y, col);
                    //dataTexture->
                    //std::cout << height << " " << rain << " " << temperature << " " << biome << "\n";
                    dataTexture->setPixel(x, y, {height, rain, temperature, biome});
                    realX++;
                }
                realY++;
            }
        }
    }
}

const glm::vec2& MapChunk::getTexturePosition() const
{
    return texturePosition;
}

void MapChunk::reset()
{
    next = 0;
}

Generator* MapChunk::generator = nullptr;
BiomeStorage* MapChunk::storage = nullptr;
std::array<glm::vec4, 5461>* MapChunk::queue = nullptr;
