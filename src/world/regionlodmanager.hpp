#pragma once
#include <fea/util.hpp>
#include "worldmessages.hpp"
#include "region.hpp"

class RegionLodManager : 
        public fea::MessageReceiver<RegionDeliverMessage>,
        public fea::MessageReceiver<RegionDeletedMessage>
{
    public:
        RegionLodManager(fea::MessageBus& bus);
        ~RegionLodManager();
        void handleMessage(const RegionDeliverMessage& message);
        void handleMessage(const RegionDeletedMessage& message);
    private:
        fea::MessageBus& mBus;
        std::unordered_map<RegionCoord, const Region&> mRegions;
};
