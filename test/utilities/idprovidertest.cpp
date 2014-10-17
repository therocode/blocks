#include "../catch.hpp"
#include <string>
#include "../../src/utilities/idprovider.hpp"

SCENARIO("IdProviders give unique IDs for unique entries, but the same IDs for the same entries.","[utilities]")
{
    GIVEN("An IdProvider")
    {
        IdProvider<std::string> provider;

        WHEN("Values that are both the same and different")
        {   
            uint32_t id1 = provider.getId("hej");
            uint32_t id2 = provider.getId("kalle");
            uint32_t id3 = provider.getId("kalle");
            uint32_t id4 = provider.getId("hej");
            uint32_t id5 = provider.getId("glass");
            uint32_t id6 = provider.getId("hej");

            THEN("The Ids that should be the same are the same, and the other ones are different")
            {
                CHECK(id1 == id4);
                CHECK(id1 == id6);
                CHECK(id2 == id3);
                CHECK_FALSE(id1 == id2);
                CHECK_FALSE(id1 == id3);
                CHECK_FALSE(id1 == id5);
                CHECK_FALSE(id2 == id5);
            }   
        }   
    }
}
