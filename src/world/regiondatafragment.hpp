#pragma once
#include "valuemap.hpp"

class RegionDataFragment
{
    public:
        RegionDataFragment(const FloatMap& heightData, const FloatMap& temperatureData, const FloatMap& rainData, const FloatMap& biomeSelectorData);
        const FloatMap& getHeightData() const;
        const FloatMap& getTemperatureData() const;
        const FloatMap& getRainData() const;
        const FloatMap& getBiomeSelectorData() const;
    public:
        FloatMap mHeightData;
        FloatMap mTemperatureData;
        FloatMap mRainData;
        FloatMap mBiomeSelectorData;
};
