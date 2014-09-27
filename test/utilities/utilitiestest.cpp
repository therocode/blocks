#define CATCH_CONFIG_MAIN
#include "../catch.hpp"
#include "../../src/utilities/directorycreator.hpp"

SCENARIO("Directories can be created, checked for existence and removed","[utilities]")
{
    WHEN("A directory is created")
    {   
        bool succeeded = DirectoryCreator::createDirectory("test_directory_1");

        THEN("The creation succeeded")
        {
            CHECK(succeeded);
        }   
    }   

    WHEN("A directory is created on an already existing directory")
    {
        bool succeeded = DirectoryCreator::createDirectory("test_directory_1");

        THEN("The creation fails")
        {
            CHECK_FALSE(succeeded);
        }
    }
            
    WHEN("A directory which existence is checked")
    {
        bool existingDirectoryExisted = DirectoryCreator::directoryExists("test_directory_1");
        bool nonexistingDirectoryDidNotExist = !DirectoryCreator::directoryExists("fantasy_directory_1");

        THEN("The checking worked")
        {
            CHECK(existingDirectoryExisted);
            CHECK(nonexistingDirectoryDidNotExist);
        }
    }

    WHEN("A directory is removed")
    {
        bool directoryRemoved = DirectoryCreator::removeDirectory("test_directory_1");

        THEN("The directory was removed")
        {
            CHECK(directoryRemoved);
        }
    }

    WHEN("A removed directory is checked for")
    {
        bool removedDirectoryDidNotExist = !DirectoryCreator::directoryExists("test_directory_1");

        THEN("The removal fails")
        {
            CHECK(removedDirectoryDidNotExist);
        }
    }
}
