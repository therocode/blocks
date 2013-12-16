#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include <tuple>

#include <featherkit/messaging.h>

#include "world/highlightmanager.h"
#include "world/worldmessages.h"

using namespace std;

class TestReceiver : 
    public fea::MessageReceiver<ChunkHighlightedMessage>,
    public fea::MessageReceiver<ChunkDehighlightedMessage>
{
    public:
        ChunkCoordinate getHighlightedLoc()
        {
            return mHighlightedLoc;
        }

        void setHighlightedLoc(ChunkCoordinate loc)
        {
            mHighlightedLoc = loc;
        }

        ChunkCoordinate getDehighlightedLoc()
        {
            return mDehighlightedLoc;
        }

        void setDehighlightedLoc(ChunkCoordinate loc)
        {
            mDehighlightedLoc = loc;
        }
    
    private:
        void handleMessage(const ChunkHighlightedMessage& message)
        {
            tie(mHighlightedLoc) = message.data;
        }

        void handleMessage(const ChunkDehighlightedMessage& message)
        {
            tie(mDehighlightedLoc) = message.data;
        } 

        ChunkCoordinate mHighlightedLoc;
        ChunkCoordinate mDehighlightedLoc; 
};

TEST_CASE("adding and removing", "[adding][removing]")
{
    fea::MessageBus bus;
    TestReceiver receiver;
    bus.addMessageSubscriber<ChunkHighlightedMessage>(receiver);
    bus.addMessageSubscriber<ChunkDehighlightedMessage>(receiver);
    HighlightManager manager(bus);
    ChunkCoordinate loc(1,1,1);
    ChunkCoordinate loc2(2,2,2);

    receiver.setHighlightedLoc(loc2);
    receiver.setDehighlightedLoc(loc2);

    SECTION("add one")
    {
        manager.addHighlight(loc);
        REQUIRE(loc == receiver.getHighlightedLoc());
        REQUIRE(loc2 == receiver.getDehighlightedLoc());
    }

    SECTION("add one remove one")
    {
        manager.addHighlight(loc);
        manager.removeHighlight(loc);
        REQUIRE(loc == receiver.getHighlightedLoc());
        REQUIRE(loc == receiver.getDehighlightedLoc());
    }

    SECTION("add two remove one")
    {
        manager.addHighlight(loc);
        manager.addHighlight(loc);
        manager.removeHighlight(loc);
        REQUIRE(loc == receiver.getHighlightedLoc());
        REQUIRE(loc2 == receiver.getDehighlightedLoc());
    }

    SECTION("add two remove two")
    {
        manager.addHighlight(loc);
        manager.addHighlight(loc);
        manager.removeHighlight(loc);
        manager.removeHighlight(loc);
        REQUIRE(loc == receiver.getHighlightedLoc());
        REQUIRE(loc == receiver.getDehighlightedLoc());
    }

    SECTION("remove one")
    {
        CHECK_THROWS_AS(manager.removeHighlight(loc), HighlightManagerException);
    }

    SECTION("add one remove two")
    {
        manager.addHighlight(loc);
        manager.removeHighlight(loc);
        CHECK_THROWS_AS(manager.removeHighlight(loc), HighlightManagerException);
    }
}
