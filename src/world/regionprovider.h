#pragma once
#include <fea/messaging.hpp>
#include "worldmessages.h"
#include "generation/regiongenerator.h"
#include "biomestorage.h"

struct Pixel
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

class RegionProvider : public fea::MessageReceiver<RegionNeededMessage>
{
    public:
        RegionProvider(fea::MessageBus& bus);
        ~RegionProvider();
        void handleMessage(const RegionNeededMessage& received) override;
    private:
        fea::MessageBus& mBus;
        RegionGenerator mRegionGenerator;
        BiomeStorage mStorage;
        std::array<Pixel, regionVoxelWidthx2> mImage;
        std::array<Pixel, regionVoxelWidthx2> mRain;
        std::array<Pixel, regionVoxelWidthx2> mTemp;
        std::array<Pixel, regionVoxelWidthx2> mHeight;
};
