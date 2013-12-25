#include "regionprovider.h"
#include "region.h"

RegionProvider::RegionProvider(fea::MessageBus& bus) : mBus(bus)
{
    mBus.addMessageSubscriber<RegionNeededMessage>(*this);


    //                                                                   temp              rain                 height
    mStorage.addBiome(new Biome("snowpeak", 24, 1.0f, 1.0f, 1.0f,        Range(0.0f, 0.1f), Range(0.0f, 1.0f),  Range(0.9f, 1.0f)));
    mStorage.addBiome(new Biome("peak", 6, 0.6f, 0.6f, 0.6f,            Range(0.1f, 1.0f), Range(0.0f, 1.0f),  Range(0.9f, 1.0f)));

    mStorage.addBiome(new Biome("baremountain", 6, 0.5f, 0.5f, 0.5f,    Range(0.0f, 1.0f), Range(0.0f, 1.0f),  Range(0.7f, 0.9f)));
    mStorage.addBiome(new Biome("grassmountain", 25, 0.4f, 0.6f, 0.4f,   Range(0.3f, 1.0f), Range(0.3f, 1.0f),  Range(0.7f, 0.9f)));
    mStorage.addBiome(new Biome("snowymountain", 24, 1.0f, 1.0f, 1.0f,   Range(0.0f, 0.3f), Range(0.4f, 1.0f),  Range(0.7f, 0.9f)));

    mStorage.addBiome(new Biome("forest", 26, 0.0f, 0.4f, 0.0f,         Range(0.35f, 1.0f),Range(0.60f, 1.0f),  Range(0.2f, 0.7f)));
    mStorage.addBiome(new Biome("taiga", 27, 0.0f, 0.6f, 0.3f,          Range(0.1f, 0.5f), Range(0.60f, 1.0f),  Range(0.2f, 0.7f)));
    mStorage.addBiome(new Biome("plains", 1, 0.2f, 0.7f, 0.0f,          Range(0.25f, 1.0f),Range(0.50f, 0.7f),  Range(0.2f, 0.7f)));
    mStorage.addBiome(new Biome("steppe", 28, 0.3f, 0.6f, 0.0f,          Range(0.25f, 1.0f), Range(0.20f, 0.60f), Range(0.2f, 0.7f)));
    mStorage.addBiome(new Biome("snowfield", 29, 1.3f, 1.3f, 1.3f,       Range(0.1f, 0.3f), Range(0.20f, 1.0f),  Range(0.2f, 0.7f)));
    mStorage.addBiome(new Biome("arcticfield", 30, 1.42f, 1.42f, 1.42f,  Range(0.0f, 0.13f), Range(0.20f, 1.0f),  Range(0.2f, 0.7f)));
    mStorage.addBiome(new Biome("xeric", 31, 1.0f, 0.8f, 0.2f,           Range(0.35f, 1.0f), Range(0.05f, 0.25f), Range(0.2f, 0.7f)));
    mStorage.addBiome(new Biome("desert", 32, 1.3f, 1.3f, 0.2f,          Range(0.35f, 1.0f), Range(0.0f, 0.15f),  Range(0.2f, 0.7f)));
    mStorage.addBiome(new Biome("coldxeric", 33, 1.f, 0.7f, 0.4f,       Range(0.0f, 0.4f), Range(0.05f, 0.25f),  Range(0.2f, 0.7f)));
    mStorage.addBiome(new Biome("colddesert", 34, 1.2f, 0.9f, 0.5f,      Range(0.0f, 0.4f), Range(0.0f, 0.15f),  Range(0.2f, 0.7f)));

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

    Region newRegion(mRegionGenerator.generateHeightmap(coordinate), mRegionGenerator.generateRainfall(coordinate), mRegionGenerator.generateTemperature(coordinate), mRegionGenerator.generateBiomeSelector(coordinate));

    VoxelTypeMap biomeTypes;

    for(int32_t y = 0; y < regionWidth * chunkWidth; y++)
    for(int32_t x = 0; x < regionWidth * chunkWidth; x++)
    {
        float height = newRegion.getHeightmap().getUnit(x, y);
        float rain = newRegion.getRainmap().getUnit(x, y);
        float temperature = newRegion.getTemperaturemap().getUnit(x, y);
        float selector = newRegion.getBiomeSelector().getUnit(x, y);

        Biome* biome = mStorage.getBiome(height, rain, temperature, selector);

        biomeTypes.setUnit(x, y, biome->mType);
    }


    newRegion.setBiomeTypes(biomeTypes);


    mBus.sendMessage(RegionDeliverMessage(coordinate, newRegion));
}
