#include "../catch.hpp"
#include "../../src/utilities/namedenum.hpp"

named_enum(TestEnum, CREAM, 
        //interrupting comment
        NOODLES, SALAD, BREAD, CURRY);

SCENARIO("named enums can be created and their values and texts are correct","[utilities]")
{
    GIVEN("a named enum")
    {
        //declared at top
        WHEN("Values and strings are stored")
        {   
            int32_t cream = CREAM;
            int32_t noodles = NOODLES;
            int32_t salad = SALAD;
            int32_t bread = BREAD;
            int32_t curry = CURRY;

            std::string creamString = TestEnumToString(cream);
            std::string noodlesString = TestEnumToString(noodles);
            std::string saladString = TestEnumToString(salad);
            std::string breadString = TestEnumToString(bread);
            std::string curryString = TestEnumToString(curry);

            THEN("The numericals and texts correspond to the expected ones")
            {
                REQUIRE(cream == 0);
                REQUIRE(noodles == 1);
                REQUIRE(salad == 2);
                REQUIRE(bread == 3);
                REQUIRE(curry == 4);

                REQUIRE(creamString == "CREAM");
                REQUIRE(noodlesString == "NOODLES");
                REQUIRE(saladString == "SALAD");
                REQUIRE(breadString == "BREAD");
                REQUIRE(curryString == "CURRY");
            }   
        }   
    }
}

named_enum(SingleEnum, KALLE);

SCENARIO("named enums can have only one element","[utilities]")
{
    GIVEN("a named enum with one entry")
    {
        //declared at top
        WHEN("Value and string is stored")
        {
            int32_t kalle = KALLE;
            std::string kalleString = SingleEnumToString(kalle);

            THEN("The value and string is correct")
            {
                REQUIRE(kalle == 0);
                REQUIRE(kalleString == "KALLE");
            }
        }
    }
}
