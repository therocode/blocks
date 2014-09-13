#include "regionmanager.hpp"

RegionManager::RegionManager(fea::MessageBus& bus) : mBus(bus)
{
    mBus.addSubscriber<RegionDeliverMessage>(*this);   
    mBus.addSubscriber<RegionDeletedMessage>(*this);   
}

RegionManager::~RegionManager()
{
    mBus.removeSubscriber<RegionDeliverMessage>(*this);   
    mBus.removeSubscriber<RegionDeletedMessage>(*this);   
}

void RegionManager::handleMessage(const RegionDeliverMessage& message)
{
    mRegions.emplace(message.coordinate, message.newRegion);
}

void RegionManager::handleMessage(const RegionDeletedMessage& message)
{
    mRegions.erase(message.coordinate);
}
