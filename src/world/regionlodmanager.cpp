#include "regionlodmanager.h"

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
    mRegions.emplace(std::get<0>(message.data), std::get<1>(message.data));
}

void RegionLodManager::handleMessage(const RegionDeletedMessage& message)
{
    mRegions.erase(std::get<0>(message.data));
}
