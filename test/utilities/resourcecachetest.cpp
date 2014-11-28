#include "../catch.hpp"
#include "../../src/utilities/resourcecache.hpp"

class IntFromStringLoader
{
    public:
        int load(const std::string& accessor)
        {
            return std::stoi(accessor);
        }
};

class FloatFromStringLoader
{
    public:
        float load(const std::string& accessor)
        {
            return std::stof(accessor);
        }
};

SCENARIO("Resource cache can be used to access different resources", "[utilities]")
{
    GIVEN("A resource cache")
    {
        ResourceCache cache;

        WHEN("Some resources are accessed")
        {
            const std::shared_ptr<int> loadedInt4 = cache.access<IntFromStringLoader>(std::string("4"));
            const std::shared_ptr<int> loadedInt1 = cache.access<IntFromStringLoader>(std::string("1"));
            const std::shared_ptr<float> loadedFloat5 = cache.access<FloatFromStringLoader>(std::string("5.0f"));
            const std::shared_ptr<float> loadedFloat6 = cache.access<FloatFromStringLoader>(std::string("6.0f"));


            THEN("The resources are available")
            {
                REQUIRE(loadedInt4 != nullptr);
                REQUIRE(loadedInt1 != nullptr);
                REQUIRE(loadedFloat5 != nullptr);
                REQUIRE(loadedFloat6 != nullptr);
                CHECK(*loadedInt4 == 4);
                CHECK(*loadedInt1 == 1);
                CHECK(*loadedFloat5 == 5.0f);
                CHECK(*loadedFloat6 == 6.0f);
            }
        }
    }
}

SCENARIO("After resources are accessed the first time in the resource cache, they are marked as loaded", "[utilities]")
{
    GIVEN("A resource cache")
    {
        ResourceCache cache;

        WHEN("Some resources are accessed")
        {
            const std::shared_ptr<int> loadedInt4 = cache.access<IntFromStringLoader>(std::string("4"));
            const std::shared_ptr<int> loadedInt1 = cache.access<IntFromStringLoader>(std::string("1"));
            const std::shared_ptr<float> loadedInt5 = cache.access<FloatFromStringLoader>(std::string("5"));
            const std::shared_ptr<float> loadedInt6 = cache.access<FloatFromStringLoader>(std::string("6"));

            THEN("The resources are marked as loaded, whilst unaccessed resources are not")
            {
                CHECK(cache.isLoaded<IntFromStringLoader>(std::string("4")));
                CHECK(cache.isLoaded<IntFromStringLoader>(std::string("1")));
                CHECK(cache.isLoaded<FloatFromStringLoader>(std::string("5")));
                CHECK(cache.isLoaded<FloatFromStringLoader>(std::string("6")));
                CHECK_FALSE(cache.isLoaded<IntFromStringLoader>(std::string("0")));
                CHECK_FALSE(cache.isLoaded<IntFromStringLoader>(std::string("2")));
                CHECK_FALSE(cache.isLoaded<FloatFromStringLoader>(std::string("0.0")));
                CHECK_FALSE(cache.isLoaded<FloatFromStringLoader>(std::string("20.1")));
            }
        }
    }
}

SCENARIO("When resources are accessed more than once, they still give out the same resource", "[utilities]")
{
    GIVEN("A resource cache with some resources accessed")
    {
        ResourceCache cache;

        cache.access<IntFromStringLoader>(std::string("4"));
        cache.access<IntFromStringLoader>(std::string("1"));
        cache.access<FloatFromStringLoader>(std::string("5.0f"));
        cache.access<FloatFromStringLoader>(std::string("6.0f"));

        WHEN("Those resources are accessed again")
        {
            const std::shared_ptr<int> loadedInt4 = cache.access<IntFromStringLoader>(std::string("4"));
            const std::shared_ptr<int> loadedInt1 = cache.access<IntFromStringLoader>(std::string("1"));
            const std::shared_ptr<float> loadedFloat5 = cache.access<FloatFromStringLoader>(std::string("5.0f"));
            const std::shared_ptr<float> loadedFloat6 = cache.access<FloatFromStringLoader>(std::string("6.0f"));


            THEN("The resources are still available")
            {
                REQUIRE(loadedInt4 != nullptr);
                REQUIRE(loadedInt1 != nullptr);
                REQUIRE(loadedFloat5 != nullptr);
                REQUIRE(loadedFloat6 != nullptr);
                CHECK(*loadedInt4 == 4);
                CHECK(*loadedInt1 == 1);
                CHECK(*loadedFloat5 == 5.0f);
                CHECK(*loadedFloat6 == 6.0f);
            }
        }
    }
}

SCENARIO("Resources can be unloaded", "[utilities]")
{
    GIVEN("A resource cache with some resources loaded")
    {
        ResourceCache cache;

        cache.access<IntFromStringLoader>(std::string("4"));
        cache.access<IntFromStringLoader>(std::string("1"));
        cache.access<FloatFromStringLoader>(std::string("5.0f"));
        cache.access<FloatFromStringLoader>(std::string("6.0f"));

        WHEN("Those resources are requested as unloaded")
        {
            cache.unload<IntFromStringLoader>(std::string("4"));
            cache.unload<IntFromStringLoader>(std::string("1"));
            cache.unload<FloatFromStringLoader>(std::string("5.0f"));
            cache.unload<FloatFromStringLoader>(std::string("6.0f"));


            THEN("The resources are not marked as loaded")
            {
                CHECK_FALSE(cache.isLoaded<IntFromStringLoader>(std::string("4")));
                CHECK_FALSE(cache.isLoaded<IntFromStringLoader>(std::string("1")));
                CHECK_FALSE(cache.isLoaded<FloatFromStringLoader>(std::string("5")));
                CHECK_FALSE(cache.isLoaded<FloatFromStringLoader>(std::string("6")));
            }
        }
    }
}

SCENARIO("Unloaded resources can be accessed again, causing them to load", "[utilities]")
{
    GIVEN("A resource cache with some resources loaded")
    {
        ResourceCache cache;

        cache.access<IntFromStringLoader>(std::string("4"));
        cache.access<IntFromStringLoader>(std::string("1"));
        cache.access<FloatFromStringLoader>(std::string("5.0f"));
        cache.access<FloatFromStringLoader>(std::string("6.0f"));

        WHEN("Those resources are requested as unloaded, but the loaded again")
        {
            cache.unload<IntFromStringLoader>(std::string("4"));
            cache.unload<IntFromStringLoader>(std::string("1"));
            cache.unload<FloatFromStringLoader>(std::string("5.0f"));
            cache.unload<FloatFromStringLoader>(std::string("6.0f"));

            const std::shared_ptr<int> loadedInt4 = cache.access<IntFromStringLoader>(std::string("4"));
            const std::shared_ptr<int> loadedInt1 = cache.access<IntFromStringLoader>(std::string("1"));
            const std::shared_ptr<float> loadedFloat5 = cache.access<FloatFromStringLoader>(std::string("5.0f"));
            const std::shared_ptr<float> loadedFloat6 = cache.access<FloatFromStringLoader>(std::string("6.0f"));

            THEN("The resources are available and marked as loaded")
            {
                CHECK(cache.isLoaded<IntFromStringLoader>(std::string("4")));
                CHECK(cache.isLoaded<IntFromStringLoader>(std::string("1")));
                CHECK(cache.isLoaded<FloatFromStringLoader>(std::string("5.0f")));
                CHECK(cache.isLoaded<FloatFromStringLoader>(std::string("6.0f")));

                REQUIRE(loadedInt4 != nullptr);
                REQUIRE(loadedInt1 != nullptr);
                REQUIRE(loadedFloat5 != nullptr);
                REQUIRE(loadedFloat6 != nullptr);
                CHECK(*loadedInt4 == 4);
                CHECK(*loadedInt1 == 1);
                CHECK(*loadedFloat5 == 5.0f);
                CHECK(*loadedFloat6 == 6.0f);
            }
        }
    }
}

SCENARIO("Non-existing resources can silently be ignored on unload", "[utilities]")
{
    GIVEN("An empty resource cache")
    {
        ResourceCache cache;

        WHEN("Unexisting resources are unloaded")
        {
            cache.unload<IntFromStringLoader>(std::string("4"));
            cache.unload<IntFromStringLoader>(std::string("1"));
            cache.unload<FloatFromStringLoader>(std::string("5.0f"));
            cache.unload<FloatFromStringLoader>(std::string("6.0f"));

            THEN("The resources are still marked as unloaded")
            {
                CHECK_FALSE(cache.isLoaded<IntFromStringLoader>(std::string("4")));
                CHECK_FALSE(cache.isLoaded<IntFromStringLoader>(std::string("1")));
                CHECK_FALSE(cache.isLoaded<FloatFromStringLoader>(std::string("5.0f")));
                CHECK_FALSE(cache.isLoaded<FloatFromStringLoader>(std::string("6.0f")));
            }
        }
    }
}
