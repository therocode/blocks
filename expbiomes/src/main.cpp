#include <fea/userinterface.hpp>
#include <fea/ui/sdlwindowbackend.hpp>
#include <fea/ui/sdlinputbackend.hpp>
#include <fea/render2d.hpp>
#include "../../src/utilities/noise.h"
#include <SFML/Graphics/Image.hpp>
#include <iostream>
#include "biomestorage.hpp"

const int seed = 19;
const int32_t partSize = 512;

int main()
{
    fea::Window window(new fea::SDLWindowBackend());
    fea::InputHandler input(new fea::SDLInputBackend());

    window.create(fea::VideoMode(512, 512, 32), "Window and user input");
    window.setFramerateLimit(60);

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
    fea::Quad square2({partSize, partSize});
    square2.setPosition({partSize, 0.0f});
    fea::Quad square3({partSize, partSize});
    square3.setPosition({0.0f, partSize});
    fea::Quad square4({partSize, partSize});
    square4.setPosition({partSize, partSize});


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
