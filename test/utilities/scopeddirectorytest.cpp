#include "../catch.hpp"
#include <fstream>
#include "../scopeddirectory.hpp"
#include "../../src/utilities/directorycreator.hpp"

SCENARIO("Scoped directory creates and removes directories properly", "[utilities]")
{
    GIVEN("A scoped directory")
    {
        {
            ScopedDirectory scoped("scoped_directory");

            WHEN("it is within the scope")
            {
                THEN("A directory for it exists")
                {
                    CHECK(DirectoryCreator::directoryExists("scoped_directory"));
                }
            }
        }

        WHEN("it is out of scope")
        {
            THEN("the directory is gone")
            {
                CHECK_FALSE(DirectoryCreator::directoryExists("scoped_directory"));
            }
        }
    }

    GIVEN("A scoped directory with a file in it")
    {
        {
            ScopedDirectory scoped("scoped_directory");

            std::ofstream file("scoped_directory/dummy");

            WHEN("it is within the scope")
            {
                THEN("A directory for it exists")
                {
                    CHECK(DirectoryCreator::directoryExists("scoped_directory"));
                }
            }
        }

        WHEN("it is out of scope")
        {
            THEN("the directory is gone")
            {
                CHECK_FALSE(DirectoryCreator::directoryExists("scoped_directory"));
            }
        }
    }
}
