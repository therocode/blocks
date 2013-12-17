#include "regionprovider.h"
#include "region.h"

RegionProvider::RegionProvider(fea::MessageBus& bus) : mBus(bus)
{
    mBus.addMessageSubscriber<RegionNeededMessage>(*this);
}

RegionProvider::~RegionProvider()
{
    mBus.removeMessageSubscriber<RegionNeededMessage>(*this);
}

void RegionProvider::handleMessage(const RegionNeededMessage& received)
{
    RegionCoordinate coordinate;

    std::tie(coordinate) = received.data;

    Region newRegion;

    mBus.sendMessage(RegionDeliverMessage(coordinate, newRegion));
}
