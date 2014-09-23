#include "regiondatafragment.hpp"

RegionDataFragment::RegionDataFragment(const FloatMap& heightData, const FloatMap& temperatureData, const FloatMap& rainData, const FloatMap& biomeSelectorData) :
    mHeightData(heightData),
    mTemperatureData(temperatureData),
    mRainData(rainData),
    mBiomeSelectorData(biomeSelectorData)
{
}

const FloatMap& RegionDataFragment::getHeightData() const
{
    return mHeightData;
}

const FloatMap& RegionDataFragment::getTemperatureData() const
{
    return mTemperatureData;
}

const FloatMap& RegionDataFragment::getRainData() const
{
    return mRainData;
}

const FloatMap& RegionDataFragment::getBiomeSelectorData() const
{
    return mBiomeSelectorData;
}
