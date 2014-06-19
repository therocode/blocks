#include <fea/userinterface.hpp>
#include <fea/ui/sfmlwindowbackend.hpp>
#include <fea/ui/sfmlinputbackend.hpp>
#include <fea/render2d.hpp>
#include "../../src/utilities/noise.h"
#include <SFML/Graphics/Image.hpp>
#include <iostream>

const int seed = 19;
const int32_t partSize = 512;

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

const size_t unitAmount = partSize * partSize;

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
            return units[x + y * partSize];
        }
        void setUnit(size_t x, size_t y, float value)
        {
            units[x + y * partSize] = value;
        }
        void toTexture(fea::Texture& texture)
        {
            for(int x = 0; x < partSize; x++)
            {
                for(int y = 0; y < partSize; y++)
                {
                    texture.setPixel(x, y, {getUnit(x, y), getUnit(x, y), getUnit(x, y)});
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
            for(int x = 0; x < partSize; x++)
            {
                for(int y = 0; y < partSize; y++)
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

                        texture.setPixel(x, y, {biome->r * height, biome->g * height, biome->b * height});
                    }
                    else
                    {
                        texture.setPixel(x, y, {0.0f, 0.0f, 0.0f});
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

void generateHeightMap(IntensityMap& map, int32_t xPos, int32_t yPos)
{
    Noise simplex(seed);
    Noise simplex2(seed + 17);
    for(int x = 0; x < partSize; x++)
    {
        for(int y = 0; y < partSize; y++)
        {
            //float value = raw_noise_3d((float) x / 200.0f, (float) y / 200.0f, 10.5);
            float value = simplex.simplexOctave2D((float) (x + xPos) / 200.0f, (float) (y + yPos) / 200.0f, 0.6, 6, 0.5f);
             value +=      simplex2.simplexOctave2D((float) (x + xPos) / 800.0f, (float) (y + yPos) / 800.0f, 0.6f, 6); //big rain difference
            //value = simplex.white2D((float) x / 2.0f, (float) y / 2.0f);

            //float value = (perlin.GetValue((float) x / 200.0f, (float) y / 200.0f, 1000.5));
            //std::cout << "value: " << value << "\n";
            value = value * 1.8f;
            value = (value + 1.0f) / 2.0f;
            value = std::max(0.0f, std::min(value, 1.0f));
            map.setUnit(x, y, value);
        }
    }
}

void generateRainfall(IntensityMap& map, const IntensityMap& heightmap, int32_t xPos, int32_t yPos)
{
    Noise simplex(seed);
    for(int x = 0; x < partSize; x++)
    {
        for(int y = 0; y < partSize; y++)
        {
           // noise::module::Voronoi voronoi;
            //voronoi.EnableDistance(true);

            //float xTurbulence = 1.0f - voronoi.GetValue((float) x / 200.0f, (float) y / 200.0f, -30.5);
            //float yTurbulence = 1.0f - voronoi.GetValue((float) x / 200.0f, (float) y / 200.0f, 850.5);

            //float invHeight = 1.0f - heightmap.getUnit(x + xTurbulence * 20, y + yTurbulence * 20);
            float value = simplex.simplexOctave3D((float) (x + xPos) / 200.0f, (float) (y + yPos) / 200.0f, 500.5, 0.6f, 6, 0.5f);
             value +=      simplex.simplexOctave3D((float) (x + xPos) / 800.0f, (float) (y + yPos) / 800.0f, 590.5, 0.6f, 6); //big rain difference

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

void generateTemperature(IntensityMap& map, const IntensityMap& heightmap, int32_t xPos, int32_t yPos)
{
    Noise simplex(seed);
    for(int x = 0; x < partSize; x++)
    {
        for(int y = 0; y < partSize; y++)
        {
            float xTurbulence = 0.0f;//perlin.GetValue((float) x / 200.0f, (float) y / 200.0f, 0.5);
            float yTurbulence = 0.0f;//perlin.GetValue((float) x / 200.0f, (float) y / 200.0f, 50.5);

            //float value = (perlin.GetValue((float) x / 200.0f + xTurbulence, (float) y / 200.0f + yTurbulence, 0.5));
            float value = simplex.simplexOctave3D((float) (x + xPos) / 200.0f, (float) (y + yPos) / 200.0f, 0.5, 0.6f, 6);
             value +=      simplex.simplexOctave3D((float) (x + xPos) / 800.0f, (float) (y + yPos) / 800.0f, 90.5, 0.6f, 6); //big temperature difference
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

void generateBiomeSelector(IntensityMap& map, int32_t xPos, int32_t yPos)
{
    Noise simplex(seed);
    for(int x = 0; x < partSize; x++)
    {
        for(int y = 0; y < partSize; y++)
        {
            float xTurbulence = simplex.simplexOctave3D((float) x / 40.0f, (float) y / 40.0f, 0.5, 0.6f, 6, 0.5f);
            float yTurbulence = simplex.simplexOctave3D((float) x / 40.0f, (float) y / 40.0f, 50.5, 0.6f, 6, 0.5f);

            //float value = (voronoi.GetValue(((float) x / 70.0f) + xTurbulence * 0.3f, ((float) y / 70.0f) + yTurbulence * 0.3f, 0.5) + 2.0f) / 3.7f;
            float value = simplex.voronoi2D((x + xPos) / 60.0f + xTurbulence * 0.25f, (y + yPos) / 60.0f + yTurbulence * 0.25f);
            map.setUnit(x, y, value);
        }
    }
}

class WorldPart
{
    public:
        WorldPart(int32_t x, int32_t y, BiomeStorage& storage) : xPos(x), yPos(y),
        generator(&heightmap, &rainfall, &temperature, &biomeSelector, storage)
        {
            generateHeightMap(heightmap, x * partSize, y * partSize);
            generateRainfall(rainfall, heightmap, x * partSize, y * partSize);
            generateTemperature(temperature, heightmap, x * partSize, y * partSize);
            generateBiomeSelector(biomeSelector, x * partSize, y * partSize);
        }
        void getHeightTexture(fea::Texture& texture)
        {
            heightmap.toTexture(texture);
        }
        void getRainfallTexture(fea::Texture& texture)
        {
            rainfall.toTexture(texture);
        }
        void getTemperatureTexture(fea::Texture& texture)
        {
            temperature.toTexture(texture);
        }
        void getBiomeSelectorTexture(fea::Texture& texture)
        {
            biomeSelector.toTexture(texture);
        }
        void getBiomeTexture(fea::Texture& texture)
        {
            generator.toTexture(texture);
        }
        void setXPosition(int32_t x)
        {
            xPos = x;
            int32_t y = yPos;
            generateHeightMap(heightmap, x * partSize, y * partSize);
            generateRainfall(rainfall, heightmap, x * partSize, y * partSize);
            generateTemperature(temperature, heightmap, x * partSize, y * partSize);
            generateBiomeSelector(biomeSelector, x * partSize, y * partSize);
        }
        void setYPosition(int32_t y)
        {
            int32_t x = xPos;
            yPos = y;
            generateHeightMap(heightmap, x * partSize, y * partSize);
            generateRainfall(rainfall, heightmap, x * partSize, y * partSize);
            generateTemperature(temperature, heightmap, x * partSize, y * partSize);
            generateBiomeSelector(biomeSelector, x * partSize, y * partSize);
        }
    private:
        int32_t xPos;
        BiomeGenerator generator;
        int32_t yPos;
        IntensityMap heightmap;
        IntensityMap rainfall;
        IntensityMap temperature;
        IntensityMap biomeSelector;
};

int main()
{
    sf::Window sfWindow;
    fea::Window window(new fea::SFMLWindowBackend(sfWindow));
    fea::InputHandler input(new fea::SFMLInputBackend(sfWindow));

    window.create(fea::VideoMode(768, 768, 32), "Window and user input");
    window.setFramerateLimit(60);

    fea::Renderer2D renderer(fea::Viewport({768, 768}, {0, 0}, fea::Camera({768.0f / 2.0f, 768.0f / 2.0f})));

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


    WorldPart part1(0, 0, storage);
    WorldPart part2(1, 0, storage);
    WorldPart part3(0, 1, storage);
    WorldPart part4(1, 1, storage);

    part1.getBiomeTexture(texture1);
    part2.getBiomeTexture(texture2);
    part3.getBiomeTexture(texture3);
    part4.getBiomeTexture(texture4);

    square1.setTexture(texture1);
    square2.setTexture(texture2);
    square3.setTexture(texture3);
    square4.setTexture(texture4);

    int32_t currentX = 0;
    int32_t currentY = 0;

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
                else if(event.key.code == fea::Keyboard::Z)
                {
                    part1.getHeightTexture(texture1);
                    square1.setColor({1.0f, 1.0f, 1.0f});
                    part2.getHeightTexture(texture2);
                    square2.setColor({1.0f, 1.0f, 1.0f});
                    part3.getHeightTexture(texture3);
                    square3.setColor({1.0f, 1.0f, 1.0f});
                    part4.getHeightTexture(texture4);
                    square4.setColor({1.0f, 1.0f, 1.0f});
                }
                else if(event.key.code == fea::Keyboard::A)
                {
                    part1.getRainfallTexture(texture1);
                    square1.setColor({0.0f, 0.6f, 1.0f});
                    part2.getRainfallTexture(texture2);
                    square2.setColor({0.0f, 0.6f, 1.0f});
                    part3.getRainfallTexture(texture3);
                    square3.setColor({0.0f, 0.6f, 1.0f});
                    part4.getRainfallTexture(texture4);
                    square4.setColor({0.0f, 0.6f, 1.0f});
                }
                else if(event.key.code == fea::Keyboard::S)
                {
                    part1.getTemperatureTexture(texture1);
                    square1.setColor({1.0f, 0.3f, 0.0f});
                    part2.getTemperatureTexture(texture2);
                    square2.setColor({1.0f, 0.3f, 0.0f});
                    part3.getTemperatureTexture(texture3);
                    square3.setColor({1.0f, 0.3f, 0.0f});
                    part4.getTemperatureTexture(texture4);
                    square4.setColor({1.0f, 0.3f, 0.0f});
                }
                else if(event.key.code == fea::Keyboard::D)
                {
                    part1.getBiomeSelectorTexture(texture1);
                    square1.setColor({1.0f, 1.0f, 1.0f});
                    part2.getBiomeSelectorTexture(texture2);
                    square2.setColor({1.0f, 1.0f, 1.0f});
                    part3.getBiomeSelectorTexture(texture3);
                    square3.setColor({1.0f, 1.0f, 1.0f});
                    part4.getBiomeSelectorTexture(texture4);
                    square4.setColor({1.0f, 1.0f, 1.0f});
                }
                else if(event.key.code == fea::Keyboard::F)
                {
                    part1.getBiomeTexture(texture1);
                    square1.setColor({1.0f, 1.0f, 1.0f});
                    part2.getBiomeTexture(texture2);
                    square2.setColor({1.0f, 1.0f, 1.0f});
                    part3.getBiomeTexture(texture3);
                    square3.setColor({1.0f, 1.0f, 1.0f});
                    part4.getBiomeTexture(texture4);
                    square4.setColor({1.0f, 1.0f, 1.0f});
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

        if(square1.getPosition().x < 0.0f)
        {
            currentX++;
            square1.translate({partSize * 2.0f, 0.0f});
            part1.setXPosition(currentX + 1);
            part1.getBiomeTexture(texture1);
        }
        if(square2.getPosition().x < 0.0f)
        {
            currentX++;
            square2.translate({partSize * 2.0f, 0.0f});
            part2.setXPosition(currentX + 1);
            part2.getBiomeTexture(texture2);
        }
        if(square3.getPosition().x < 0.0f)
        {
            square3.translate({partSize * 2.0f, 0.0f});
            part3.setXPosition(currentX + 1);
            part3.getBiomeTexture(texture3);
        }
        if(square4.getPosition().x < 0.0f)
        {
            square4.translate({partSize * 2.0f, 0.0f});
            part4.setXPosition(currentX + 1);
            part4.getBiomeTexture(texture4);
        }

        if(square1.getPosition().x > partSize * 2)
        {
            currentX--;
            square1.translate({-partSize * 2.0f, 0.0f});
            part1.setXPosition(currentX);
            part1.getBiomeTexture(texture1);
        }
        if(square2.getPosition().x > partSize * 2)
        {
            currentX--;
            square2.translate({-partSize * 2.0f, 0.0f});
            part2.setXPosition(currentX);
            part2.getBiomeTexture(texture2);
        }
        if(square3.getPosition().x > partSize * 2)
        {
            square3.translate({-partSize * 2.0f, 0.0f});
            part3.setXPosition(currentX);
            part3.getBiomeTexture(texture3);
        }
        if(square4.getPosition().x > partSize * 2)
        {
            square4.translate({-partSize * 2.0f, 0.0f});
            part4.setXPosition(currentX);
            part4.getBiomeTexture(texture4);
        }

        if(square1.getPosition().y < 0.0f)
        {
            currentY++;
            square1.translate({0.0f, partSize * 2.0f});
            part1.setYPosition(currentY + 1);
            part1.getBiomeTexture(texture1);
        }
        if(square2.getPosition().y < 0.0f)
        {
            square2.translate({0.0f, partSize * 2.0f});
            part2.setYPosition(currentY + 1);
            part2.getBiomeTexture(texture2);
        }
        if(square3.getPosition().y < 0.0f)
        {
            currentY++;
            square3.translate({0.0f, partSize * 2.0f});
            part3.setYPosition(currentY + 1);
            part3.getBiomeTexture(texture3);
        }
        if(square4.getPosition().y < 0.0f)
        {
            square4.translate({0.0f, partSize * 2.0f});
            part4.setYPosition(currentY + 1);
            part4.getBiomeTexture(texture4);
        }

        if(square1.getPosition().y > partSize * 2)
        {
            currentY--;
            square1.translate({0.0f, -partSize * 2.0f});
            part1.setYPosition(currentY);
            part1.getBiomeTexture(texture1);
        }
        if(square2.getPosition().y > partSize * 2)
        {
            square2.translate({0.0f, -partSize * 2.0f});
            part2.setYPosition(currentY);
            part2.getBiomeTexture(texture2);
        }
        if(square3.getPosition().y > partSize * 2)
        {
            currentY--;
            square3.translate({0.0f, -partSize * 2.0f});
            part3.setYPosition(currentY);
            part3.getBiomeTexture(texture3);
        }
        if(square4.getPosition().y > partSize * 2)
        {
            square4.translate({0.0f, -partSize * 2.0f});
            part4.setYPosition(currentY);
            part4.getBiomeTexture(texture4);
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
