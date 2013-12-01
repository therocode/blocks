#include <featherkit/userinterface.h>
#include <featherkit/util/window/sfml/sfmlwindowbackend.h>
#include <featherkit/util/input/sfml/sfmlinputbackend.h>
#include <featherkit/render2d.h>
#include "../../src/utilities/simplexnoise.h"
#include <SFML/Graphics/Image.hpp>
#include <noise.h>
#include <iostream>

const int seed = 14;

struct Range
{
    Range(float mi, float ma) : min(mi), max(ma) {};
    bool isWithin(float v) 
    {
        return v >= min && v <= max;
    }
    float min;
    float max;
};

struct Biome
{
    Biome(const std::string& name, float red, float green, float blue, Range temp, Range rain, Range height) : r(red), g(green), b(blue), temperatureRange(temp), rainfallRange(rain), heightRange(height) {};
    float r;
    float g;
    float b;
    Range temperatureRange;
    Range rainfallRange;
    Range heightRange;
};

class BiomeStorage
{
    public:
        Biome* getBiome(float temperature, float rainfall, float height, float selector)
        {
            std::vector<Biome*> possibleBiomes(biomes.size(), nullptr);

            if(biomes.size() > 0)
            {
                std::copy_if(biomes.begin(), biomes.end(), possibleBiomes.begin(), [=] (Biome* biome) {return biome->temperatureRange.isWithin(temperature) && biome->rainfallRange.isWithin(rainfall) && biome->heightRange.isWithin(height);});

                for(uint32_t i = 0; i < possibleBiomes.size(); i++)
                {
                    if(possibleBiomes[i] == nullptr)
                    {
                        possibleBiomes.erase(possibleBiomes.begin() + i);
                        i--;
                    }
                }

                if(possibleBiomes.size() != 0)
                {
                    size_t index = 0;

                    index = possibleBiomes.size() * selector;

                    return possibleBiomes[index];
                }
            }

            return nullptr;
        }
        void addBiome(Biome* biome)
        {
            biomes.push_back(biome);
        }
    private:
        std::vector<Biome*> biomes; 
};

const size_t unitAmount = 800 * 600;

class IntensityMap
{
    public:
        IntensityMap()
        {
            units = new float[unitAmount];
        }
        ~IntensityMap()
        {
            delete [] units;
        }
        float getUnit(size_t x, size_t y) const
        {
            return units[x + y * 800];
        }
        void setUnit(size_t x, size_t y, float value)
        {
            units[x + y * 800] = value;
        }
        void toTexture(fea::Texture& texture)
        {
            for(int x = 0; x < 800; x++)
            {
                for(int y = 0; y < 600; y++)
                {
                    texture.setPixel(x, y, getUnit(x, y), getUnit(x, y), getUnit(x, y));
                }
            }
            texture.update();
        }
    private:
        float* units;
};

class BiomeGenerator
{
    public:
        BiomeGenerator(IntensityMap* h, IntensityMap* r, IntensityMap* t, IntensityMap* b, BiomeStorage& s) : heightmap(h), rainfall(r), temperature(t), biomeSelector(b), storage(s) {};
        void toTexture(fea::Texture& texture)
        {
            for(int x = 0; x < 800; x++)
            {
                for(int y = 0; y < 600; y++)
                {
                    float temp = temperature->getUnit(x, y);
                    float rain = rainfall->getUnit(x, y);
                    float selector = biomeSelector->getUnit(x, y);
                    float height = heightmap->getUnit(x, y);

                    Biome* biome = storage.getBiome(temp, rain, height, selector);

                    if(biome)
                    {
                        if(height < 0.7f)
                            height = 0.7f;

                        texture.setPixel(x, y, biome->r * height, biome->g * height, biome->b * height);
                    }
                    else
                    {
                        texture.setPixel(x, y, 0.0f, 0.0f, 0.0f);
                    }
                }
            }
            texture.update();
        }
    private:
        IntensityMap* heightmap;
        IntensityMap* rainfall;
        IntensityMap* temperature;
        IntensityMap* biomeSelector;
        BiomeStorage& storage;
};

void generateHeightMap(IntensityMap& map)
{
    for(int x = 0; x < 800; x++)
    {
        for(int y = 0; y < 600; y++)
        {
            noise::module::Perlin perlin;
            perlin.SetSeed(seed);
            //float value = raw_noise_3d((float) x / 200.0f, (float) y / 200.0f, 10.5);
            float value = octave_noise_3d(6, 0.5f, 0.6f, (float) x / 200.0f, (float) y / 200.0f, 10.5);

            //float value = (perlin.GetValue((float) x / 200.0f, (float) y / 200.0f, 1000.5));
            //std::cout << "value: " << value << "\n";
            value = value * 1.8f;
            value = (value + 1.0f) / 2.0f;
            value = std::max(0.0f, std::min(value, 1.0f));
            map.setUnit(x, y, value);
        }
    }
}

void generateRainfall(IntensityMap& map, const IntensityMap& heightmap)
{
    for(int x = 0; x < 800; x++)
    {
        for(int y = 0; y < 600; y++)
        {
            noise::module::Perlin perlin;
            perlin.SetSeed(seed);
           // noise::module::Voronoi voronoi;
            //voronoi.EnableDistance(true);

            //float xTurbulence = 1.0f - voronoi.GetValue((float) x / 200.0f, (float) y / 200.0f, -30.5);
            //float yTurbulence = 1.0f - voronoi.GetValue((float) x / 200.0f, (float) y / 200.0f, 850.5);

            //float invHeight = 1.0f - heightmap.getUnit(x + xTurbulence * 20, y + yTurbulence * 20);
            float value = octave_noise_3d(6, 0.5f, 0.6f, (float) x / 200.0f, (float) y / 200.0f, 500.5);

            //float value = (perlin.GetValue((float) x / 200.0f, (float) y / 200.0f, 500.5));
            //std::cout << "value: " << value << "\n";
            value = value * 1.8f;
            value = (value + 1.0f) / 2.0f;
            value = value - heightmap.getUnit(x, y) / 4.0f;
            value = std::max(0.0f, std::min(value, 1.0f));
            map.setUnit(x, y, value);
        }
    }
}

void generateTemperature(IntensityMap& map, const IntensityMap& heightmap)
{
    for(int x = 0; x < 800; x++)
    {
        for(int y = 0; y < 600; y++)
        {
            noise::module::Perlin perlin;
            perlin.SetSeed(seed);
            float xTurbulence = 0.0f;//perlin.GetValue((float) x / 200.0f, (float) y / 200.0f, 0.5);
            float yTurbulence = 0.0f;//perlin.GetValue((float) x / 200.0f, (float) y / 200.0f, 50.5);

            //float value = (perlin.GetValue((float) x / 200.0f + xTurbulence, (float) y / 200.0f + yTurbulence, 0.5));
            float value = octave_noise_3d(6, 0.5f, 0.6f, (float) x / 200.0f, (float) y / 200.0f, 0.5);
            //std::cout << "value: " << value << "\n";
            value = value * 1.2f + 0.2f;
            value = (value + 1.0f) / 2.0f;
            //value = value * (float)(y - 100) / 299.0f; //height fix
            value = value - heightmap.getUnit(x, y) / 2.0f;
            value = std::max(0.0f, std::min(value, 1.0f));


            map.setUnit(x, y, value);
        }
    }
}

void generateBiomeSelector(IntensityMap& map)
{
    for(int x = 0; x < 800; x++)
    {
        for(int y = 0; y < 600; y++)
        {
            noise::module::Perlin perlin;
            perlin.SetSeed(seed);
            float xTurbulence = perlin.GetValue((float) x / 40.0f, (float) y / 50.0f, 0.5);
            float yTurbulence = perlin.GetValue((float) x / 40.0f, (float) y / 50.0f, 50.5);

            noise::module::Voronoi voronoi;
            voronoi.SetSeed(seed);
            voronoi.EnableDistance(false);
            float value = (voronoi.GetValue(((float) x / 70.0f) + xTurbulence * 0.3f, ((float) y / 70.0f) + yTurbulence * 0.3f, 0.5) + 2.0f) / 3.7f;
            value = std::max(0.0f, std::min(value, 1.0f));
            map.setUnit(x, y, value);
        }
    }
}

int main()
{
    sf::Window sfWindow;
    fea::Window window(new fea::util::SFMLWindowBackend(sfWindow));
    fea::InputHandler input(new fea::util::SFMLInputBackend(sfWindow));

    window.create(fea::VideoMode(800, 600, 32), "Window and user input");
    window.setFramerateLimit(60);

    fea::Renderer2D renderer(fea::Viewport(800, 600, 0, 0, fea::Camera(800.0f / 2.0f, 600.0f / 2.0f)));

    renderer.setup();

    bool shutDown = false;

    fea::Texture biomeSelectorTexture;
    biomeSelectorTexture.create(800, 600, glm::vec3(1.0f, 0.0f, 1.0f), false, true);
    fea::Texture heightmapTexture;
    heightmapTexture.create(800, 600, glm::vec3(1.0f, 0.0f, 1.0f), false, true);
    fea::Texture rainfallTexture;
    rainfallTexture.create(800, 600, glm::vec3(1.0f, 0.0f, 1.0f), false, true);
    fea::Texture temperatureTexture;
    temperatureTexture.create(800, 600, glm::vec3(1.0f, 0.0f, 1.0f), false, true);
    fea::Texture biomeTexture;
    biomeTexture.create(800, 600, glm::vec3(1.0f, 0.0f, 1.0f), false, true);

    IntensityMap heightmap;
    generateHeightMap(heightmap);
    heightmap.toTexture(heightmapTexture);

    IntensityMap rainfall;
    generateRainfall(rainfall, heightmap);
    rainfall.toTexture(rainfallTexture);

    IntensityMap temperature;
    generateTemperature(temperature, heightmap);
    temperature.toTexture(temperatureTexture);

    IntensityMap biomeSelector;
    generateBiomeSelector(biomeSelector);
    biomeSelector.toTexture(biomeSelectorTexture);

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
 
    BiomeGenerator generator(&heightmap, &rainfall, &temperature, &biomeSelector, storage);
    generator.toTexture(biomeTexture);

    fea::Quad square(800.0f, 600.0f);
    square.setTexture(biomeTexture);


    while(!shutDown)
    {
        input.processEvents();
        
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
                else if(event.key.code == fea::Keyboard::Z)
                {
                    square.setTexture(heightmapTexture);
                    square.setColour(1.0f, 1.0f, 1.0f);
                }
                else if(event.key.code == fea::Keyboard::A)
                {
                    square.setTexture(rainfallTexture);
                    square.setColour(0.0f, 0.6f, 1.0f);
                }
                else if(event.key.code == fea::Keyboard::S)
                {
                    square.setTexture(temperatureTexture);
                    square.setColour(1.0f, 0.3f, 0.0f);
                }
                else if(event.key.code == fea::Keyboard::D)
                {
                    square.setTexture(biomeSelectorTexture);
                    square.setColour(1.0f, 1.0f, 1.0f);
                }
                else if(event.key.code == fea::Keyboard::F)
                {
                    square.setTexture(biomeTexture);
                    square.setColour(1.0f, 1.0f, 1.0f);
                }
            }
        }

        renderer.clear();
        renderer.queue(square);
        renderer.render();
        window.swapBuffers();
    }
    window.close();
}
