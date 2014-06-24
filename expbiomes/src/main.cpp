#include <fea/userinterface.hpp>
#include <fea/ui/sdlwindowbackend.hpp>
#include <fea/ui/sdlinputbackend.hpp>
#include <fea/render2d.hpp>
#include <fea/rendering/textsurface.hpp>
#include "../../src/utilities/noise.h"
#include <iostream>
#include "biomestorage.hpp"
#include "generator.hpp"
#include "mapchunk.hpp"
#include <fea/structure.hpp>
#include <algorithm>

class BiomeApp : public fea::Application
{
    public:
        void setup(const std::vector<std::string>& arg) override;
        void loop() override;
        void destroy() override{};
};

int main()
{
    BiomeApp app;
    app.run();
}

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

fea::Window window(new fea::SDLWindowBackend());
fea::InputHandler input(new fea::SDLInputBackend());

fea::Renderer2D renderer(fea::Viewport({512, 512}, {0, 0}, fea::Camera({512.0f / 2.0f, 512.0f / 2.0f})));

fea::Texture texture1;
fea::Texture texture2;
fea::Texture texture3;
fea::Texture texture4;
fea::Texture dataTexture1;
fea::Texture dataTexture2;
fea::Texture dataTexture3;
fea::Texture dataTexture4;

BiomeStorage storage;

fea::Quad square1({partSize, partSize});
fea::Quad square2({partSize, partSize});
fea::Quad square3({partSize, partSize});
fea::Quad square4({partSize, partSize});

std::array<fea::Quad*, 4> quads;
std::unordered_map<fea::Quad*, glm::vec2> coords;
std::unordered_map<fea::Quad*, fea::Texture*> textures;
std::unordered_map<fea::Quad*, fea::Texture*> dataTextures;
std::array<glm::vec4, 5461> rectQueue;

Generator generator;


std::unordered_map<glm::ivec2, MapChunk> mapChunks;

std::unordered_map<fea::Quad*, std::vector<glm::ivec2>> squareChunks;

float zoom = 1.0f;

std::string biomeName("none");

fea::Font font;
fea::TextSurface text;
int32_t elevation = 0;
int32_t degrees = 0;
int32_t rainfall = 0;

fea::Quad textBg;

bool displayText = false;

void updateText(const glm::ivec2& position)
{
    text.setPosition((glm::vec2)position);
    text.setPenFont(font);
    text.clear();
    
    text.setLineHeight(14.0f);
    text.setPenColor(fea::Color::White);
    text.setPenPosition({0.0f, 12.0f});
    text.write(biomeName);

    text.newLine();
    text.write("Elevation: " + std::to_string(elevation));
    text.newLine();
    text.write("Temperature: " + std::to_string(degrees) + " C");
    text.newLine();
    text.write("Rainfall: " + std::to_string(rainfall) + " mm");
    textBg.setPosition((glm::vec2)position);
    textBg.setSize(text.getSize());
}

void BiomeApp::setup(const std::vector<std::string>& args)
{

    MapChunk::generator = &generator;
    MapChunk::storage = &storage;
    MapChunk::queue = &rectQueue;
    Generator::zoom = &zoom;

    window.create(fea::VideoMode(512, 512, 32), "Window and user input");
    window.setFramerateLimit(60);
    window.setVSyncEnabled(true);

    renderer.setup();

    font = fea::Font("data/arial.ttf", 14);

    textBg.setColor(fea::Color::Gray);

    texture1.create(partSize, partSize, fea::Color(1.0f, 0.0f, 1.0f), false, true);
    texture2.create(partSize, partSize, fea::Color(1.0f, 0.0f, 1.0f), false, true);
    texture3.create(partSize, partSize, fea::Color(1.0f, 0.0f, 1.0f), false, true);
    texture4.create(partSize, partSize, fea::Color(1.0f, 0.0f, 1.0f), false, true);
    dataTexture1.create(partSize, partSize, fea::Color(1.0f, 0.0f, 1.0f), false, true);
    dataTexture2.create(partSize, partSize, fea::Color(1.0f, 0.0f, 1.0f), false, true);
    dataTexture3.create(partSize, partSize, fea::Color(1.0f, 0.0f, 1.0f), false, true);
    dataTexture4.create(partSize, partSize, fea::Color(1.0f, 0.0f, 1.0f), false, true);
    //
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

    storage.addBiome(new Biome("Snowy peak", 1.0f, 1.0f, 1.0f,        Range(0.0f, 0.1f), Range(0.0f, 1.0f),  Range(0.9f, 1.0f)));
    storage.addBiome(new Biome("Peak", 0.6f, 0.6f, 0.6f,            Range(0.1f, 1.0f), Range(0.0f, 1.0f),  Range(0.9f, 1.0f)));

    storage.addBiome(new Biome("Bare mountain", 0.5f, 0.5f, 0.5f,    Range(0.0f, 1.0f), Range(0.0f, 1.0f),  Range(0.7f, 0.9f)));
    storage.addBiome(new Biome("Grass mountain", 0.4f, 0.6f, 0.4f,   Range(0.3f, 1.0f), Range(0.3f, 1.0f),  Range(0.7f, 0.9f)));
    storage.addBiome(new Biome("Snowy mountain", 1.0f, 1.0f, 1.0f,   Range(0.0f, 0.3f), Range(0.4f, 1.0f),  Range(0.7f, 0.9f)));

    storage.addBiome(new Biome("Forest", 0.0f, 0.4f, 0.0f,         Range(0.35f, 1.0f),Range(0.60f, 1.0f),  Range(0.2f, 0.7f)));
    storage.addBiome(new Biome("Taiga", 0.0f, 0.6f, 0.3f,          Range(0.1f, 0.5f), Range(0.60f, 1.0f),  Range(0.2f, 0.7f)));
    storage.addBiome(new Biome("Plains", 0.2f, 0.7f, 0.0f,          Range(0.25f, 1.0f),Range(0.50f, 0.7f),  Range(0.2f, 0.7f)));
    storage.addBiome(new Biome("Steppe", 0.3f, 0.6f, 0.0f,          Range(0.25f, 1.0f), Range(0.20f, 0.60f), Range(0.2f, 0.7f)));
    storage.addBiome(new Biome("Snowy field", 1.3f, 1.3f, 1.3f,       Range(0.1f, 0.3f), Range(0.20f, 1.0f),  Range(0.2f, 0.7f)));
    storage.addBiome(new Biome("Arctic field", 1.42f, 1.42f, 1.42f,  Range(0.0f, 0.13f), Range(0.20f, 1.0f),  Range(0.2f, 0.7f)));
    storage.addBiome(new Biome("Xeric shrubland", 1.0f, 0.8f, 0.2f,           Range(0.35f, 1.0f), Range(0.05f, 0.25f), Range(0.2f, 0.7f)));
    storage.addBiome(new Biome("Desert", 1.3f, 1.3f, 0.2f,          Range(0.35f, 1.0f), Range(0.0f, 0.15f),  Range(0.2f, 0.7f)));
    storage.addBiome(new Biome("Frozen shrubland", 1.f, 0.7f, 0.4f,       Range(0.0f, 0.4f), Range(0.05f, 0.25f),  Range(0.2f, 0.7f)));
    storage.addBiome(new Biome("Cold desert", 1.2f, 0.9f, 0.5f,      Range(0.0f, 0.4f), Range(0.0f, 0.15f),  Range(0.2f, 0.7f)));

    storage.addBiome(new Biome("Ocean", 0.0f, 0.0f, 1.0f,           Range(0.2f, 1.0f), Range(0.0f, 1.0f),  Range(0.0f, 0.2f)));
    storage.addBiome(new Biome("Arctic ocean", 0.0f, 0.9f, 1.0f,    Range(0.0f, 0.2f), Range(0.0f, 1.0f),  Range(0.0f, 0.2f)));

    square1.setPosition({0.0f, 0.0f});
    square1.setColor(fea::Color::Blue); 
    square2.setPosition(glm::vec2(partSize, 0.0f));
    square2.setColor(fea::Color::Yellow); 
    square3.setPosition(glm::vec2(0.0f, partSize));
    square3.setColor(fea::Color::Red); 
    square4.setPosition({partSize, partSize});
    square4.setColor(fea::Color::Green); 

    quads = {&square1, &square2, &square3, &square4};
    coords[&square1] = square1.getPosition();
    coords[&square2] = square2.getPosition();
    coords[&square3] = square3.getPosition();
    coords[&square4] = square4.getPosition();
    textures[&square1] = &texture1;
    textures[&square2] = &texture2;
    textures[&square3] = &texture3;
    textures[&square4] = &texture4;
    dataTextures[&square1] = &dataTexture1;
    dataTextures[&square2] = &dataTexture2;
    dataTextures[&square3] = &dataTexture3;
    dataTextures[&square4] = &dataTexture4;

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

    std::random_shuffle(rectQueue.begin() + 1, rectQueue.begin()    + 1    + 4);
    std::random_shuffle(rectQueue.begin() + 5, rectQueue.begin()    + 5    + 16);
    std::random_shuffle(rectQueue.begin() + 21, rectQueue.begin()   + 21   + 64);
    std::random_shuffle(rectQueue.begin() + 85, rectQueue.begin()   + 85   + 256);
    std::random_shuffle(rectQueue.begin() + 341, rectQueue.begin()  + 341  + 1024);
    std::random_shuffle(rectQueue.begin() + 1365, rectQueue.begin() + 1365 + 4096);

    //for(int32_t y = 0; y < 8; y++)
    //{
    //    float yy = (float) y / 8.0f;
    //    for(int32_t x = 0; x < 8; x++)
    //    {
    //        float xx = (float) x / 8.0f;

    //        MapChunk chunk;
    //        chunk.setTexture(&texture1);
    //        chunk.setTexturePosition({(float) x * 64.0f, (float) y * 64.0f});
    //        chunk.generate(glm::vec2(xx, yy) * 512.0f, {xx, yy}, {xx + 1.0f / 8.0f, yy + 1.0f / 8.0f}, 1);
    //        mapChunks.emplace(glm::ivec2(x,y), chunk);
    //        squareChunks[&square1].push_back(glm::ivec2(x,y));
    //    }
    //}
    for(auto& quad : quads)
    {
        glm::ivec2 gridPosition = (glm::ivec2)(coords[quad] / 64.0f);

        for(int32_t y = 0; y < 8; y++)
        {
            float yy = (float) y / 8.0f;
            for(int32_t x = 0; x < 8; x++)
            {
                float xx = (float) x / 8.0f;

                MapChunk chunk;
                chunk.setTexture(textures[quad], dataTextures[quad]);
                chunk.setTexturePosition({(float) x * 64.0f, (float) y * 64.0f});
                chunk.setQuad(quad);
                mapChunks.emplace(gridPosition + glm::ivec2(x,y), chunk);
                squareChunks[quad].push_back(gridPosition + glm::ivec2(x,y));
            }
        }
    }

    texture1.update();
    square1.setTexture(texture1);
    square1.setColor(fea::Color::White);
    square2.setTexture(texture2);
    square2.setColor(fea::Color::White);
    square3.setTexture(texture3);
    square3.setColor(fea::Color::White);
    square4.setTexture(texture4);
    square4.setColor(fea::Color::White);

}

void BiomeApp::loop()
{
    fea::Event event;
    while(input.pollEvent(event))
    {
        if(event.type == fea::Event::CLOSED)
        {
            quit();
        }
        else if(event.type == fea::Event::KEYPRESSED)
        {
            if(event.key.code == fea::Keyboard::Q || event.key.code == fea::Keyboard::ESCAPE)
            {
                quit();
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
            else if(event.key.code == fea::Keyboard::Z)
            {
                zoom *= 0.9f;

                for(auto& chunk : mapChunks)
                    chunk.second.reset();
            }
            else if(event.key.code == fea::Keyboard::X)
            {
                zoom /= 0.9f;

                for(auto& chunk : mapChunks)
                    chunk.second.reset();
            }
        }
        else if(event.type == fea::Event::MOUSEBUTTONPRESSED)
        {
            displayText = true;
            for(auto& quad : quads)
            {
                glm::vec2 pos = quad->getPosition();
                if(event.mouseButton.x > pos.x && event.mouseButton.y > pos.y && event.mouseButton.x < pos.x + 512.0f && event.mouseButton.y < pos.y + 512.0f)
                {
                    quad->setColor(fea::Color::White);
                    glm::ivec2 pixelCoord(event.mouseButton.x - pos.x, event.mouseButton.y - pos.y);

                    fea::Color data = dataTextures[quad]->getPixel(pixelCoord.x, pixelCoord.y);

                    float temp = data.bAsFloat();
                    float rain = data.gAsFloat();
                    float height = data.rAsFloat();
                    float selector = data.aAsFloat(); 

                    elevation = 6000 * height- 1200;
                    rainfall = 400 * rain;
                    degrees = 90 * temp - 40;
                    biomeName = storage.getBiome(temp, rain, height, selector)->name;

                    //std::cout << "this place is " << metres << " metres above ocean level\n";
                    //std::cout << "this place has " << rainfall << " cm annual precipitation\n";
                    //std::cout << "this place has the average temperature of " << degree << " degrees celcius during daytime\n";
                    //std::cout << "this biome is " << name << "\n";

                    updateText({event.mouseButton.x, event.mouseButton.y});
                }
            }
        }
        else if(event.type == fea::Event::MOUSEBUTTONRELEASED)
        {
            displayText = false;
        }
        else if(event.type == fea::Event::MOUSEMOVED)
        {
            if(displayText)
            {
                for(auto& quad : quads)
                {
                    glm::vec2 pos = quad->getPosition();
                    if(event.mouseMove.x > pos.x && event.mouseMove.y > pos.y && event.mouseMove.x < pos.x + 512.0f && event.mouseMove.y < pos.y + 512.0f)
                    {
                        quad->setColor(fea::Color::White);
                        glm::ivec2 pixelCoord(event.mouseMove.x - pos.x, event.mouseMove.y - pos.y);

                        fea::Color data = dataTextures[quad]->getPixel(pixelCoord.x, pixelCoord.y);

                        float temp = data.bAsFloat();
                        float rain = data.gAsFloat();
                        float height = data.rAsFloat();
                        float selector = data.aAsFloat(); 

                        elevation = 6000 * height- 1200;
                        rainfall = 400 * rain;
                        degrees = 90 * temp - 40;
                        biomeName = storage.getBiome(temp, rain, height, selector)->name;

                        //std::cout << "this place is " << metres << " metres above ocean level\n";
                        //std::cout << "this place has " << rainfall << " cm annual precipitation\n";
                        //std::cout << "this place has the average temperature of " << degree << " degrees celcius during daytime\n";
                        //std::cout << "this biome is " << name << "\n";

                        updateText({event.mouseMove.x, event.mouseMove.y});
                    }
                }
            }
        }
    }

    for(auto& quad : quads)
    {
        const glm::vec2& position = quad->getPosition();

        bool removed = false;

        if(position.x < -512.0f)
        {
            quad->translate({1024.0f, 0.0f});
            coords[quad] += glm::vec2(1024.0f, 0.0f);

            removed = true;
        }
        if(position.x > 512.0f)
        {
            quad->translate({-1024.0f, 0.0f});
            coords[quad] += glm::vec2(-1024.0f, 0.0f);

            removed = true;
        }
        if(position.y < -512.0f)
        {
            quad->translate({0.0f, 1024.0f});
            coords[quad] += glm::vec2(0.0f, 1024.0f);

            removed = true;
        }
        if(position.y > 512.0f)
        {
            quad->translate({0.0f, -1024.0f});
            coords[quad] += glm::vec2(0.0f, -1024.0f);

            removed = true;
        }

        if(removed)
        {
            for(auto& chunk : squareChunks[quad])
                mapChunks.erase(chunk);

            squareChunks[quad].clear();

            glm::ivec2 gridPosition = (glm::ivec2)(coords[quad] / 64.0f);

            for(int32_t y = 0; y < 8; y++)
            {
                float yy = (float) y / 8.0f;
                for(int32_t x = 0; x < 8; x++)
                {
                    float xx = (float) x / 8.0f;

                    MapChunk chunk;
                    chunk.setTexture(textures[quad], dataTextures[quad]);
                    chunk.setTexturePosition({(float) x * 64.0f, (float) y * 64.0f});
                    chunk.setQuad(quad);
                    mapChunks.emplace(gridPosition + glm::ivec2(x,y), chunk);
                    squareChunks[quad].push_back(gridPosition + glm::ivec2(x,y));
                }
            }
        }
    }

    for(auto& chunk : mapChunks)
    {
        float xx = (float) chunk.first.x / 8.0f;
        float yy = (float) chunk.first.y / 8.0f;

        glm::vec2 screenPos((glm::vec2)chunk.second.getTexturePosition() + chunk.second.getQuad()->getPosition());

        if(screenPos.x > -64.0f && screenPos.y > -64.0f && screenPos.x < 512.0f && screenPos.y < 512.0f)
            chunk.second.generate(glm::vec2(xx, yy) * 512.0f, {xx, yy}, {xx + 1.0f / 8.0f, yy + 1.0f / 8.0f}, 30);
    }

    texture1.update();
    texture2.update();
    texture3.update();
    texture4.update();

    renderer.clear();
    renderer.queue(square1);
    renderer.queue(square2);
    renderer.queue(square3);
    renderer.queue(square4);

    if(displayText)
    {
        renderer.queue(textBg);
        renderer.queue(text);
    }

    renderer.render();
    window.swapBuffers();
}
