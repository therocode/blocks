#include "regionlodmanager.hpp"

RegionLodManager::RegionLodManager(fea::MessageBus& bus) : mBus(bus)
{
    mBus.addSubscriber<RegionDeliverMessage>(*this);   
    mBus.addSubscriber<RegionDeletedMessage>(*this);   
}

RegionLodManager::~RegionLodManager()
{
    mBus.removeSubscriber<RegionDeliverMessage>(*this);   
    mBus.removeSubscriber<RegionDeletedMessage>(*this);   
}

void RegionLodManager::handleMessage(const RegionDeliverMessage& message)
{
    mRegions.emplace(message.coordinate, message.newRegion);
}

void RegionLodManager::handleMessage(const RegionDeletedMessage& message)
{
    mRegions.erase(message.coordinate);
}
