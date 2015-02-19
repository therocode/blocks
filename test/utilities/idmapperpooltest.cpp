#include "../catch.hpp"
#include <string>
#include "../../src/utilities/idmapper.hpp"
#include "../../src/utilities/idpool.hpp"

SCENARIO("IdMappers give unique IDs for unique entries, but the same IDs for the same entries.","[utilities]")
{
    GIVEN("An IdMapper")
    {
        IdMapper<std::string> provider;

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

SCENARIO("getIdConst can be used to fetch existing entries from IDMapper","[utilities]")
{
    GIVEN("An IdMapper")
    {
        IdMapper<std::string> provider;

        WHEN("Values that are both the same and different")
        {   
            uint32_t id1 = provider.getId("hej");
            uint32_t id2 = provider.getId("kalle");
            uint32_t id3 = provider.getIdConst("kalle");
            uint32_t id4 = provider.getIdConst("hej");
            uint32_t id5 = provider.getId("glass");
            uint32_t id6 = provider.getIdConst("hej");

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

SCENARIO("IdMappers reuse freed IDs","[utilities]")
{
    GIVEN("An IdMapper with some values added")
    {
        IdMapper<std::string> provider;

        uint32_t id1 = provider.getId("hej");
        uint32_t id2 = provider.getId("dej");
        uint32_t id3 = provider.getId("sej");

        WHEN("Ids are freed and more values assigned")
        {   
            provider.free("hej");
            provider.free("sej");

            uint32_t id4 = provider.getId("lax");
            uint32_t id5 = provider.getId("kex");
            uint32_t id6 = provider.getId("boll");

            THEN("Those IDs are reused, but only reused once")
            {

                CHECK(id5 == id1);
                CHECK(id4 == id3);
                CHECK_FALSE(id6 == id1);
                CHECK_FALSE(id6 == id2);
                CHECK_FALSE(id6 == id3);
                CHECK_FALSE(id6 == id4);
                CHECK_FALSE(id6 == id5);
            }   
        }   
    }
}

SCENARIO("You can ask an IDMapper if a value is already given an ID","[utilities]")
{
    GIVEN("An IdMapper with values added")
    {
        IdMapper<std::string> provider;

        uint32_t id1 = provider.getId("hej");
        uint32_t id2 = provider.getId("kalle");
        uint32_t id3 = provider.getId("glass");


        WHEN("Existing and unexisting values are polled for")
        {   
            bool existing1 = provider.valueExists("hej");
            bool existing2 = provider.valueExists("kalle");
            bool existing3 = provider.valueExists("glass");
            bool unexisting1 = provider.valueExists("goj");
            bool unexisting2 = provider.valueExists("lapp");
            bool unexisting3 = provider.valueExists("teckna");

            THEN("The status is correct")
            {
                CHECK(existing1);
                CHECK(existing2);
                CHECK(existing3);
                CHECK_FALSE(unexisting1);
                CHECK_FALSE(unexisting2);
                CHECK_FALSE(unexisting3);
            }   
        }   
    }
}

SCENARIO("IdMappers can give out the value given to an ID","[utilities]")
{
    GIVEN("An IdMapper with values added")
    {
        IdMapper<std::string> provider;

        uint32_t id1 = provider.getId("hej");
        uint32_t id2 = provider.getId("kalle");
        uint32_t id3 = provider.getId("glass");


        WHEN("Original values are requested by the given ID")
        {   
            std::string val1 = provider.valueFromId(id1);
            std::string val2 = provider.valueFromId(id2);
            std::string val3 = provider.valueFromId(id3);

            THEN("They are properly received")
            {
                CHECK(val1 == "hej");
                CHECK(val2 == "kalle");
                CHECK(val3 == "glass");
            }   
        }   
    }
}

SCENARIO("IdPools give unique IDs for subsequent requests.","[utilities]")
{
    GIVEN("An IdPool")
    {
        IdPool<uint32_t> provider;

        WHEN("Some values are gotten")
        {   
            std::set<int32_t> ids;

            for(int32_t i = 0; i < 100; i++)
                ids.insert(provider.next());

            THEN("The values are unique")
            {
                CHECK(ids.size() == 100);
            }   
        }   
    }
}

SCENARIO("IdPools give out returned IDs but still only unique entries.","[utilities]")
{
    GIVEN("An IdPool")
    {
        IdPool<uint32_t> provider;

        WHEN("Some values are gotten and returned and gotten again")
        {   
            std::set<int32_t> ids;

            for(int32_t i = 0; i < 100; i++)
                ids.insert(provider.next());

            std::set<int32_t> returned = {1, 34, 23, 10, 55};
            for(int32_t id : returned)
            {
                provider.free(id);
                ids.erase(id);
            }

            for(int32_t id: returned)
            {
                ids.insert(provider.next());
            }


            THEN("The values are unique, and the returned values are present")
            {
                CHECK(ids.size() == 100);
            }   
        }   
    }
}
