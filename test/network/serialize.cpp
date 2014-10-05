#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include "../src/networking/networkingprotocol.hpp"
#include "../src/networking/messageserializer.hpp"

const double accuracy = 0.0002;

bool closeEnough(double a, double b)
{
    return fabs(a - b) < accuracy; 
}

SCENARIO("Networking messages can be serialized and deserialized","[networking]")
{
    GIVEN("Network messages with data")
    {   
        TestMessage1 test1{"This is a cool message!", 54};

        TestMessage2 test2
        {
            {
                {"kalle", {1, 2}},
                {"ralle", {5, 0, -3}}
            },
            0.00003
        };

        WHEN("Messages are serialized and deserialized")
        {   
            ByteVector test1bytes = serializeMessage(test1);
            ByteVector test2bytes = serializeMessage(test2);

            int32_t deserialized1Type = decodeType(test1bytes);
            int32_t deserialized2Type = decodeType(test2bytes);

            TestMessage1 deserialized1 = deserializeMessage<TestMessage1>(test1bytes);
            TestMessage2 deserialized2 = deserializeMessage<TestMessage2>(test2bytes);

            THEN("The deserialized types are correct and the deserialized messages matches the original ones")
            {
                CHECK(deserialized1Type == TEST_1);
                CHECK(deserialized2Type == TEST_2);

                CHECK(deserialized1.text == "This is a cool message!");
                CHECK(deserialized1.numerical == 54);

                CHECK(deserialized2.textNumbersMap.count("kalle") == 1);
                CHECK(deserialized2.textNumbersMap.count("ralle") == 1);

                if(deserialized2.textNumbersMap.count("kalle") == 1)
                {
                    CHECK(deserialized2.textNumbersMap.at("kalle")[0] == 1);
                    CHECK(deserialized2.textNumbersMap.at("kalle")[1] == 2);
                }

                if(deserialized2.textNumbersMap.count("ralle") == 1)
                {
                    CHECK(deserialized2.textNumbersMap.at("ralle")[0] == 5);
                    CHECK(deserialized2.textNumbersMap.at("ralle")[1] == 0);
                    CHECK(deserialized2.textNumbersMap.at("ralle")[2] == -3);
                }

                CHECK(closeEnough(deserialized2.floatNumerical, 0.00003));
            }   
        }   
    }   
}
