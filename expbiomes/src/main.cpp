#include <fea/userinterface.hpp>
#include <fea/ui/sdlwindowbackend.hpp>
#include <fea/ui/sdlinputbackend.hpp>
#include <fea/render2d.hpp>
#include "../../src/utilities/noise.h"
#include <SFML/Graphics/Image.hpp>
#include <iostream>
#include "biomestorage.hpp"
#include "generator.hpp"
#include "mapchunk.hpp"

namespace std 
{
    template<>
        struct hash<glm::ivec2>
        {   
            public:
                std::size_t operator()(glm::ivec2 const& coord) const 
                {   
                    /*  
                    size_t x = coord.x;

                                      x = 00000000 00000000 00000000 00000000 01234567 89abcdef ghijklmn opqrstuv
                                          01234567 89abcdef ghijklmn opqrstuv 00000000 00000000 00000000 00000000 << 32
                                          01234567 89abcdef ghijklmn opqrstuv 01234567 89abcdef ghijklmn opqrstuv | x
                                          11111111 11111111 00000000 00000000 00000000 00000000 11111111 11111111 &
                                      x = 01234567 89abcdef 00000000 00000000 00000000 00000000 ghijklmn opqrstuv
                                          00000000 00000000 00000000 00000000 ghijklmn opqrstuv 00000000 00000000 << 16
                                          01234567 89abcdef 00000000 00000000 ghijklmn opqrstuv ghijklmn opqrstuv | x
                                          00000000 11111111 00000000 00000000 11111111 00000000 00000000 11111111 &
                                      x = 00000000 89abcdef 00000000 00000000 ghijklmn 00000000 00000000 opqrstuv 
                                          89abcdef 00000000 00000000 ghijklmn 00000000 00000000 opqrstuv 00000000 << 8
                                          89abcdef 89abcdef 00000000 ghijklmn ghijklmn 00000000 opqrstuv opqrstuv | x
                                          11110000 00001111 00000000 11110000 00001111 00000000 11110000 00001111 &
                                      x = 89ab0000 0000cdef 00000000 ghij0000 0000klmn 00000000 opqr0000 0000stuv
                                          00000000 cdef0000 0000ghij 00000000 klmn0000 0000opqr 00000000 stuv0000 << 4
                                          89ab0000 cdefcdef 0000ghij ghij0000 klmnklmn 0000opqr opqr0000 stuvstuv | x
                                          00110000 11000011 00001100 00110000 11000011 00001100 00110000 11000011 &
                                      x = 00ab0000 cd0000ef 0000gh00 00ij0000 kl0000mn 0000op00 00qr0000 st0000uv
                                          ab0000cd 0000ef00 00gh0000 ij0000kl 0000mn00 00op0000 qr0000st 0000uv00 << 2
                                          abab00cd cd00efef 00ghgh00 ijij00kl kl00mnmn 00opop00 qrqr00st st00uvuv | x
                                          10010010 01001001 00100100 10010010 01001001 00100100 10010010 01001001 &
                                      x = a00b00c0 0d00e00f 00g00h00 i00j00k0 0l00m00n 00o00p00 q00r00s0 0t00u00v
                                      */
                    size_t x = coord.x;
                    x = (x | (x << 16)) & 0x00FF0000FF0000FF;
                    x = (x | (x << 8))  & 0xF00F00F00F00F00F;
                    x = (x | (x << 4))  & 0x30C30C30C30C30C3;
                    x = (x | (x << 2))  & 0x9249249249249249;

                    size_t y = coord.y;
                    y = (y | (y << 16)) & 0x00FF0000FF0000FF;
                    y = (y | (y << 8))  & 0xF00F00F00F00F00F;
                    y = (y | (y << 4))  & 0x30C30C30C30C30C3;
                    y = (y | (y << 2))  & 0x9249249249249249;

                    return x | (y << 1); 
                }   
        };  
}

const int seed = 19;
const int32_t partSize = 512;

int main()
{
    fea::Window window(new fea::SDLWindowBackend());
    fea::InputHandler input(new fea::SDLInputBackend());

    window.create(fea::VideoMode(512, 512, 32), "Window and user input");
    window.setFramerateLimit(60);
    window.setVSyncEnabled(true);

    fea::Renderer2D renderer(fea::Viewport({512, 512}, {0, 0}, fea::Camera({512.0f / 2.0f, 512.0f / 2.0f})));

    renderer.setup();

    bool shutDown = false;

    fea::Texture texture1;
    texture1.create(partSize, partSize, fea::Color(1.0f, 0.0f, 1.0f), false, true);
    fea::Texture texture2;
    texture2.create(partSize, partSize, fea::Color(1.0f, 0.0f, 1.0f), false, true);
    fea::Texture texture3;
    texture3.create(partSize, partSize, fea::Color(1.0f, 0.0f, 1.0f), false, true);
    fea::Texture texture4;
    texture4.create(partSize, partSize, fea::Color(1.0f, 0.0f, 1.0f), false, true);

    BiomeStorage storage;
    //temp             //rain
    //storage.addBiome(new Biome("forest", 0.1f, 0.5f, 0.0f,          Range(0.3f, 0.8f), Range(0.4f, 1.0f),  Range(0.2f, 1.0f)));
    //storage.addBiome(new Biome("coniferousforest", 0.0f, 0.5f, 0.3f,Range(0.0f, 0.5f), Range(0.4f, 1.0f),  Range(0.2f, 1.0f)));
    //storage.addBiome(new Biome("rainforest", 0.0f, 0.3f, 0.0f,      Range(0.6f, 1.0f), Range(0.55f, 0.8f), Range(0.2f, 1.0f)));
    //storage.addBiome(new Biome("hotswamp", 0.0f, 0.2f, 0.2f,        Range(0.6f, 1.0f), Range(0.8f, 1.0f),  Range(0.2f, 1.0f)));
    //storage.addBiome(new Biome("steppe", 0.2f, 0.7f, 0.0f,          Range(0.3f, 1.0f), Range(0.2f, 0.8f),  Range(0.2f, 1.0f)));
    //storage.addBiome(new Biome("snowplains", 1.0f, 1.0f, 1.0f,      Range(0.0f, 0.3f), Range(0.2f, 0.7f),  Range(0.2f, 1.0f)));
    //storage.addBiome(new Biome("drysteppe", 0.4f, 0.6f, 0.0f,       Range(0.4f, 0.8f), Range(0.05f, 0.5f), Range(0.2f, 1.0f)));
    //storage.addBiome(new Biome("sandydesert", 1.0f, 0.8f, 0.0f,     Range(0.3f, 1.0f), Range(0.0f, 0.2f),  Range(0.2f, 1.0f)));
    //storage.addBiome(new Biome("arcticdesert", 1.0f, 0.8f, 0.5f,    Range(0.0f, 0.3f), Range(0.0f, 0.2f),  Range(0.2f, 1.0f)));

    storage.addBiome(new Biome("snowpeak", 1.0f, 1.0f, 1.0f,        Range(0.0f, 0.1f), Range(0.0f, 1.0f),  Range(0.9f, 1.0f)));
    storage.addBiome(new Biome("peak", 0.6f, 0.6f, 0.6f,            Range(0.1f, 1.0f), Range(0.0f, 1.0f),  Range(0.9f, 1.0f)));

    storage.addBiome(new Biome("baremountain", 0.5f, 0.5f, 0.5f,    Range(0.0f, 1.0f), Range(0.0f, 1.0f),  Range(0.7f, 0.9f)));
    storage.addBiome(new Biome("grassmountain", 0.4f, 0.6f, 0.4f,   Range(0.3f, 1.0f), Range(0.3f, 1.0f),  Range(0.7f, 0.9f)));
    storage.addBiome(new Biome("snowymountain", 1.0f, 1.0f, 1.0f,   Range(0.0f, 0.3f), Range(0.4f, 1.0f),  Range(0.7f, 0.9f)));

    storage.addBiome(new Biome("forest", 0.0f, 0.4f, 0.0f,         Range(0.35f, 1.0f),Range(0.60f, 1.0f),  Range(0.2f, 0.7f)));
    storage.addBiome(new Biome("taiga", 0.0f, 0.6f, 0.3f,          Range(0.1f, 0.5f), Range(0.60f, 1.0f),  Range(0.2f, 0.7f)));
    storage.addBiome(new Biome("plains", 0.2f, 0.7f, 0.0f,          Range(0.25f, 1.0f),Range(0.50f, 0.7f),  Range(0.2f, 0.7f)));
    storage.addBiome(new Biome("steppe", 0.3f, 0.6f, 0.0f,          Range(0.25f, 1.0f), Range(0.20f, 0.60f), Range(0.2f, 0.7f)));
    storage.addBiome(new Biome("snowfield", 1.3f, 1.3f, 1.3f,       Range(0.1f, 0.3f), Range(0.20f, 1.0f),  Range(0.2f, 0.7f)));
    storage.addBiome(new Biome("arcticfield", 1.42f, 1.42f, 1.42f,  Range(0.0f, 0.13f), Range(0.20f, 1.0f),  Range(0.2f, 0.7f)));
    storage.addBiome(new Biome("xeric", 1.0f, 0.8f, 0.2f,           Range(0.35f, 1.0f), Range(0.05f, 0.25f), Range(0.2f, 0.7f)));
    storage.addBiome(new Biome("desert", 1.3f, 1.3f, 0.2f,          Range(0.35f, 1.0f), Range(0.0f, 0.15f),  Range(0.2f, 0.7f)));
    storage.addBiome(new Biome("coldxeric", 1.f, 0.7f, 0.4f,       Range(0.0f, 0.4f), Range(0.05f, 0.25f),  Range(0.2f, 0.7f)));
    storage.addBiome(new Biome("colddesert", 1.2f, 0.9f, 0.5f,      Range(0.0f, 0.4f), Range(0.0f, 0.15f),  Range(0.2f, 0.7f)));

    storage.addBiome(new Biome("ocean", 0.0f, 0.0f, 1.0f,           Range(0.2f, 1.0f), Range(0.0f, 1.0f),  Range(0.0f, 0.2f)));
    storage.addBiome(new Biome("arctic ocean", 0.0f, 0.9f, 1.0f,    Range(0.0f, 0.2f), Range(0.0f, 1.0f),  Range(0.0f, 0.2f)));

    fea::Quad square1({partSize, partSize});
    square1.setPosition({0.0f, 0.0f});
    square1.setColor(fea::Color::Blue); 
    fea::Quad square2({partSize, partSize});
    square2.setPosition({partSize, 0.0f});
    square2.setColor(fea::Color::Yellow); 
    fea::Quad square3({partSize, partSize});
    square3.setPosition({0.0f, partSize});
    square3.setColor(fea::Color::Red); 
    fea::Quad square4({partSize, partSize});
    square4.setPosition({partSize, partSize});
    square4.setColor(fea::Color::Green); 

    std::array<fea::Quad*, 4> quads = {&square1, &square2, &square3, &square4};
    std::array<glm::vec2, 4 > coords= {square1.getPosition(),
        square2.getPosition(),
        square3.getPosition(),
        square4.getPosition()};

    std::array<glm::vec4, 5461> rectQueue;
    int32_t rectQueueIterator = 0;

    for(int32_t i = 0; i < 7; i++)
    {
        int32_t gridWidth = std::pow(2, i);
        float width = 1.0f / (float) gridWidth;

        for(int32_t y = 0; y < gridWidth; y++)
        {
            for(int32_t x = 0; x < gridWidth; x++)
            {
                rectQueue[rectQueueIterator] = glm::vec4((float)x * width,
                        (float)y * width,
                        (float)x * width + width,
                        (float)y * width + width);
                rectQueueIterator++;
            }
        }
    }

    Generator generator;

    MapChunk::generator = &generator;
    MapChunk::storage = &storage;
    MapChunk::queue = &rectQueue;

    std::unordered_map<glm::ivec2, MapChunk> mapChunks;

    for(int32_t y = 0; y < 8; y++)
    {
        float yy = (float) y / 8.0f;
        for(int32_t x = 0; x < 8; x++)
        {
            float xx = (float) x / 8.0f;

            MapChunk chunk;
            chunk.generate(glm::vec2(xx, yy) * 512.0f, {xx, yy}, {xx + 1.0f / 8.0f, yy + 1.0f / 8.0f}, texture1, 1);
            mapChunks.emplace(glm::ivec2(x,y), chunk);
        }
    }

    texture1.update();
    square1.setTexture(texture1);
    square1.setColor(fea::Color::White);

    while(!shutDown)
    {
        fea::Event event;
        while(input.pollEvent(event))
        {
            if(event.type == fea::Event::CLOSED)
            {
                shutDown = true;
            }
            else if(event.type == fea::Event::KEYPRESSED)
            {
                if(event.key.code == fea::Keyboard::Q || event.key.code == fea::Keyboard::ESCAPE)
                {
                    shutDown = true;
                }
                else if(event.key.code == fea::Keyboard::UP)
                {
                    square1.translate({0.0f, 5.0f});
                    square2.translate({0.0f, 5.0f});
                    square3.translate({0.0f, 5.0f});
                    square4.translate({0.0f, 5.0f});
                }
                else if(event.key.code == fea::Keyboard::DOWN)
                {
                    square1.translate({0.0f, -5.0f});
                    square2.translate({0.0f, -5.0f});
                    square3.translate({0.0f, -5.0f});
                    square4.translate({0.0f, -5.0f});
                }
                else if(event.key.code == fea::Keyboard::LEFT)
                {
                    square1.translate({5.0f, 0.0f});
                    square2.translate({5.0f, 0.0f});
                    square3.translate({5.0f, 0.0f});
                    square4.translate({5.0f, 0.0f});
                }
                else if(event.key.code == fea::Keyboard::RIGHT)
                {
                    square1.translate({-5.0f, 0.0f});
                    square2.translate({-5.0f, 0.0f});
                    square3.translate({-5.0f, 0.0f});
                    square4.translate({-5.0f, 0.0f});
                }
            }
        }

        int32_t i = 0;
        for(auto quad : quads)
        {
            const glm::vec2& position = quad->getPosition();

            if(position.x < -512.0f)
            {
                quad->translate({1024.0f, 0.0f});
                coords[i] += glm::vec2(1024.0f, 0.0f);
            }
            if(position.x > 512.0f)
            {
                quad->translate({-1024.0f, 0.0f});
                coords[i] += glm::vec2(-1024.0f, 0.0f);
            }
            if(position.y < -512.0f)
            {
                quad->translate({0.0f, 1024.0f});
                coords[i] += glm::vec2(0.0f, 1024.0f);
            }
            if(position.y > 512.0f)
            {
                quad->translate({0.0f, -1024.0f});
                coords[i] += glm::vec2(0.0f, -1024.0f);
            }
            i++;
        }

        for(auto& chunk : mapChunks)
        {
            float xx = (float) chunk.first.x / 8.0f;
            float yy = (float) chunk.first.y / 8.0f;
            chunk.second.generate(glm::vec2(xx, yy) * 512.0f, {xx, yy}, {xx + 1.0f / 8.0f, yy + 1.0f / 8.0f}, texture1, 51);
        }

        texture1.update();

        renderer.clear();
        renderer.queue(square1);
        renderer.queue(square2);
        renderer.queue(square3);
        renderer.queue(square4);
        renderer.render();
        window.swapBuffers();
    }
    window.close();
}
