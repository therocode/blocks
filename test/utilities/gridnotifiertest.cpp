#include "../catch.hpp"
#include "../../src/utilities/gridnotifier.hpp"

SCENARIO("Grid notifiers has all deactivated cells on start","[utilities]")
{
    WHEN("A grid notifier is created")
    {   
        GridNotifier<5> gridNotifier;

        THEN("The grid contains no active cells")
        {
            REQUIRE_FALSE(gridNotifier.isActive({0, 0, 0}));
            REQUIRE_FALSE(gridNotifier.isActive({100, 0, 0}));
            REQUIRE_FALSE(gridNotifier.isActive({0, 30, 0}));
            REQUIRE_FALSE(gridNotifier.isActive({0, 20, 19}));
        }   
    }   
}

SCENARIO("Grid notifiers show correct status of big cells","[utilities]")
{
    WHEN("A cell is activated")
    {   
        GridNotifier<10> gridNotifier;
        gridNotifier.set({15, 105, -45}, true);

        THEN("The correct big cell is activated and its neighbours are not")
        {
            REQUIRE(gridNotifier.isActive({1, 10, -5}));
            REQUIRE_FALSE(gridNotifier.isActive({2, 10, -5}));
            REQUIRE_FALSE(gridNotifier.isActive({0, 10, -5}));
            REQUIRE_FALSE(gridNotifier.isActive({1, 11, -5}));
            REQUIRE_FALSE(gridNotifier.isActive({1, 9, -5}));
            REQUIRE_FALSE(gridNotifier.isActive({1, 10, -4}));
            REQUIRE_FALSE(gridNotifier.isActive({1, 10, -6}));
            REQUIRE_FALSE(gridNotifier.isActive({2, 11, -4}));
            REQUIRE_FALSE(gridNotifier.isActive({0, 9, -6}));
        }   
    }   

    WHEN("A cell is activated and deactivated")
    {   
        GridNotifier<9> gridNotifier;
        gridNotifier.set({10, 5, -4}, true);
        gridNotifier.set({10, 5, -4}, false);

        THEN("The correct big cell is not activated")
        {
            REQUIRE_FALSE(gridNotifier.isActive({1, 0, -1}));
        }   
    }   
}

SCENARIO("Grid notifiers return notifications of activity in big cells", "[utilities]")
{
    WHEN("Cells are set to active")
    {
        GridNotifier<2> gridNotifier;

        auto activatedCell = gridNotifier.set({0, 2, 0}, true);

        THEN("The correct big cell is notified as active")
        {
            REQUIRE(activatedCell.size() > 0);
            REQUIRE(activatedCell[0] == glm::i64vec3(0, 1, 0));
        }
    }

    WHEN("Cells are set to active and then a neighbour within the same big cell is set to active")
    {
        GridNotifier<3> gridNotifier;

        gridNotifier.set({1, 2, 0}, true);
        auto activatedCell = gridNotifier.set({2, 0, 2}, true);

        THEN("The correct big cell is not notified as active")
        {
            REQUIRE(activatedCell.size() == 0);
        }
    }
}

SCENARIO("Grid notifiers return notifications of inactivity in big cells", "[utilities]")
{
    WHEN("Active cells are set to inactive")
    {
        GridNotifier<4> gridNotifier;

        gridNotifier.set({4, 5, -6}, true);
        auto deactivatedCell = gridNotifier.set({4, 5, -6}, false);

        THEN("The correct big cell is notified as unactive")
        {
            REQUIRE(deactivatedCell.size() > 0);
            REQUIRE(deactivatedCell[0] == glm::i64vec3(1, 1, -2));
        }
    }

    WHEN("Two cells in one big cell are set to active and one of them is set to inactive")
    {
        GridNotifier<5> gridNotifier;

        gridNotifier.set({0, 0, 0}, true);
        gridNotifier.set({2, 2, 2}, true);
        auto deactivatedCell = gridNotifier.set({0, 0, 0}, false);

        THEN("The correct big cell is not notified as inactive")
        {
            REQUIRE(deactivatedCell.size() == 0);
        }
    }

    WHEN("Two cells in one big cell are set to active and both of them are set to inactive")
    {
        GridNotifier<7> gridNotifier;

        gridNotifier.set({8, 3, 15}, true);
        gridNotifier.set({9, 4, 16}, true);
        gridNotifier.set({8, 3, 15}, false);
        auto deactivatedCell = gridNotifier.set({9, 4, 16}, false);

        THEN("The correct big cell is notified as inactive")
        {
            REQUIRE(deactivatedCell.size() > 0);
            REQUIRE(deactivatedCell[0] == glm::i64vec3(1, 0, 2));
        }
    }
}
