#pragma once
#include <fea/util.hpp>
#include "worldmessages.hpp"
#include "region.hpp"

class RegionManager : 
        public fea::MessageReceiver<RegionDeliverMessage>,
        public fea::MessageReceiver<RegionDeletedMessage>
{
    public:
        RegionManager(fea::MessageBus& bus);
        ~RegionManager();
        void handleMessage(const RegionDeliverMessage& message);
        void handleMessage(const RegionDeletedMessage& message);
    private:
        fea::MessageBus& mBus;
        std::unordered_map<RegionCoord, const Region&> mRegions;
};
