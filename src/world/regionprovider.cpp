#include "regionprovider.h"
#include "region.h"
#include "utilities/lodepng.h"
#include <unordered_map>

RegionProvider::RegionProvider(fea::MessageBus& bus) : mBus(bus)
{
    mBus.addMessageSubscriber<RegionNeededMessage>(*this);


    //                                                                   temp              rain                 height
    mStorage.addBiome(new Biome("snowpeak", 24, 1.0f, 1.0f, 1.0f,        Range(0.0f, 0.1f), Range(0.0f, 1.0f),  Range(0.85f, 1.0f)));
    mStorage.addBiome(new Biome("peak", 6, 0.6f, 0.6f, 0.6f,            Range(0.1f, 1.0f), Range(0.0f, 1.0f),  Range(0.85f, 1.0f)));

    mStorage.addBiome(new Biome("baremountain", 6, 0.5f, 0.5f, 0.5f,    Range(0.0f, 1.0f), Range(0.0f, 1.0f),  Range(0.68f, 0.93f)));
    mStorage.addBiome(new Biome("grassmountain", 25, 0.4f, 0.6f, 0.4f,   Range(0.3f, 1.0f), Range(0.3f, 1.0f),  Range(0.68f, 0.93f)));
    mStorage.addBiome(new Biome("snowymountain", 24, 1.0f, 1.0f, 1.0f,   Range(0.0f, 0.3f), Range(0.4f, 1.0f),  Range(0.68f, 0.93f)));

    mStorage.addBiome(new Biome("forest", 26, 0.0f, 0.4f, 0.0f,         Range(0.35f, 1.0f),Range(0.60f, 1.0f),  Range(0.2f, 0.73f)));
    mStorage.addBiome(new Biome("taiga", 27, 0.0f, 0.6f, 0.3f,          Range(0.1f, 0.5f), Range(0.60f, 1.0f),  Range(0.2f, 0.73f)));
    mStorage.addBiome(new Biome("plains", 1, 0.2f, 0.7f, 0.0f,          Range(0.25f, 1.0f),Range(0.50f, 0.7f),  Range(0.2f, 0.73f)));
    mStorage.addBiome(new Biome("steppe", 28, 0.3f, 0.6f, 0.0f,          Range(0.25f, 1.0f), Range(0.20f, 0.60f), Range(0.2f, 0.73f)));
    mStorage.addBiome(new Biome("snowfield", 29, 1.3f, 1.3f, 1.3f,       Range(0.1f, 0.3f), Range(0.20f, 1.0f),  Range(0.2f, 0.73f)));
    mStorage.addBiome(new Biome("arcticfield", 30, 1.42f, 1.42f, 1.42f,  Range(0.0f, 0.13f), Range(0.20f, 1.0f),  Range(0.2f, 0.73f)));
    mStorage.addBiome(new Biome("xeric", 31, 1.0f, 0.8f, 0.2f,           Range(0.35f, 1.0f), Range(0.05f, 0.25f), Range(0.2f, 0.73f)));
    mStorage.addBiome(new Biome("desert", 32, 1.3f, 1.3f, 0.2f,          Range(0.35f, 1.0f), Range(0.0f, 0.15f),  Range(0.2f, 0.73f)));
    mStorage.addBiome(new Biome("coldxeric", 33, 1.f, 0.7f, 0.4f,       Range(0.0f, 0.4f), Range(0.05f, 0.25f),  Range(0.2f, 0.73f)));
    mStorage.addBiome(new Biome("colddesert", 34, 1.2f, 0.9f, 0.5f,      Range(0.0f, 0.4f), Range(0.0f, 0.15f),  Range(0.2f, 0.73f)));

    mStorage.addBiome(new Biome("ocean", 35, 0.0f, 0.0f, 1.0f,           Range(0.2f, 1.0f), Range(0.0f, 1.0f),  Range(0.0f, 0.2f)));
    mStorage.addBiome(new Biome("arctic ocean", 36, 0.0f, 0.9f, 1.0f,    Range(0.0f, 0.2f), Range(0.0f, 1.0f),  Range(0.0f, 0.2f)));
}

RegionProvider::~RegionProvider()
{
    mBus.removeMessageSubscriber<RegionNeededMessage>(*this);
}

void RegionProvider::handleMessage(const RegionNeededMessage& received)
{
    RegionCoord coordinate;

    std::tie(coordinate) = received.data;

    IntensityMap height = mRegionGenerator.generateHeightmap(coordinate);
    Region newRegion(height, mRegionGenerator.generateRainfall(coordinate), mRegionGenerator.generateTemperature(coordinate, height), mRegionGenerator.generateBiomeSelector(coordinate));


    ValueMap<uint16_t> biomeIndices;
    uint16_t nextBiomeIndex = 0;
    std::unordered_map<Biome*, uint16_t> usedBiomes;

    for(int32_t y = 0; y < regionVoxelWidth; y++)
    for(int32_t x = 0; x < regionVoxelWidth; x++)
    {
        float height = newRegion.getHeightmap().getUnit(x, y);
        float rain = newRegion.getRainmap().getUnit(x, y);
        float temperature = newRegion.getTemperaturemap().getUnit(x, y);
        float selector = newRegion.getBiomeSelector().getUnit(x, y);

        Biome* biome = mStorage.getBiome(temperature, rain, height, selector);
        
        auto content = usedBiomes.find(biome);
        if(content == usedBiomes.end())
        {
            usedBiomes.emplace(biome, nextBiomeIndex);
            biomeIndices.setUnit(x, y, nextBiomeIndex);
            nextBiomeIndex++;
        }
        else
        {
            biomeIndices.setUnit(x, y, content->second);
        }

        //mImage[x + (regionVoxelWidth - y -1)*(regionVoxelWidth)].r = biome->r * 255;
        //mImage[x + (regionVoxelWidth - y -1)*(regionVoxelWidth)].g = biome->g * 255;
        //mImage[x + (regionVoxelWidth - y -1)*(regionVoxelWidth)].b = biome->b * 255;
        //mImage[x + (regionVoxelWidth - y -1)*(regionVoxelWidth)].a = 255;
        ////mImage[x + (regionVoxelWidth - y -1)*(regionVoxelWidth)].r = newRegion.getBiomeSelector().getUnit(x, y) * 255;
        ////mImage[x + (regionVoxelWidth - y -1)*(regionVoxelWidth)].g = 0;
        ////mImage[x + (regionVoxelWidth - y -1)*(regionVoxelWidth)].b = 0;
        ////mImage[x + (regionVoxelWidth - y -1)*(regionVoxelWidth)].a = 255;
        //mRain[x + (regionVoxelWidth - y -1)*(regionVoxelWidth)].r = 0;
        //mRain[x + (regionVoxelWidth - y -1)*(regionVoxelWidth)].g = 0;
        //mRain[x + (regionVoxelWidth - y -1)*(regionVoxelWidth)].b = newRegion.getRainmap().getUnit(x, y) * 255;
        //mRain[x + (regionVoxelWidth - y -1)*(regionVoxelWidth)].a = 255;
        //mTemp[x + (regionVoxelWidth - y -1)*(regionVoxelWidth)].r = newRegion.getTemperaturemap().getUnit(x, y) * 255;
        //mTemp[x + (regionVoxelWidth - y -1)*(regionVoxelWidth)].g = 0;
        //mTemp[x + (regionVoxelWidth - y -1)*(regionVoxelWidth)].b = 0;
        //mTemp[x + (regionVoxelWidth - y -1)*(regionVoxelWidth)].a = 255;
        //mHeight[x + (regionVoxelWidth - y -1)*(regionVoxelWidth)].r = newRegion.getHeightmap().getUnit(x, y) * 255;
        //mHeight[x + (regionVoxelWidth - y -1)*(regionVoxelWidth)].g = newRegion.getHeightmap().getUnit(x, y) * 255;
        //mHeight[x + (regionVoxelWidth - y -1)*(regionVoxelWidth)].b = newRegion.getHeightmap().getUnit(x, y) * 255;
        //mHeight[x + (regionVoxelWidth - y -1)*(regionVoxelWidth)].a = 255;
    }

    std::unordered_map<uint16_t, Biome*> usedBiomesFlipped;

    for(auto entry : usedBiomes)
    {
        usedBiomesFlipped.emplace(entry.second, entry.first);
    }

    newRegion.setBiomes(biomeIndices, usedBiomesFlipped);

    mBus.sendMessage(RegionDeliverMessage(coordinate, newRegion));

    //lodepng_encode32_file(std::string("regiona" + std::to_string(coordinate.x) + "_" + std::to_string(coordinate.y) + ".png").c_str(), (uint8_t*) mImage.data(), regionVoxelWidth, regionVoxelWidth);
    //lodepng_encode32_file(std::string("regionr" + std::to_string(coordinate.x) + "_" + std::to_string(coordinate.y) + ".png").c_str(), (uint8_t*) mRain.data(), regionVoxelWidth, regionVoxelWidth);
    //lodepng_encode32_file(std::string("regiont" + std::to_string(coordinate.x) + "_" + std::to_string(coordinate.y) + ".png").c_str(), (uint8_t*) mTemp.data(), regionVoxelWidth, regionVoxelWidth);
    //lodepng_encode32_file(std::string("regionh" + std::to_string(coordinate.x) + "_" + std::to_string(coordinate.y) + ".png").c_str(), (uint8_t*) mHeight.data(), regionVoxelWidth, regionVoxelWidth);
}
