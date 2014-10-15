#include "../catch.hpp"
#include <fstream>
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

    WHEN("A non-existing directory is removed")
    {
        bool directoryRemoved = DirectoryCreator::removeDirectory("fantasy_directory_1");

        THEN("The removal fails")
        {
            CHECK_FALSE(directoryRemoved);
        }
    }
}

SCENARIO("Directories with things in them can not be removed unless the force flag is set","[utilities]")
{
    GIVEN("a directory with a file in it")
    {
        DirectoryCreator::createDirectory("non_empty_directory");

        std::ofstream("non_empty_directory/dummy");

        WHEN("the directory is removed without forcing")
        {
            bool succeeded = DirectoryCreator::removeDirectory("non_empty_directory");

            THEN("the removal fails")
            {
                CHECK_FALSE(succeeded);
            }
        }

        WHEN("the directory is removed with forcing")
        {
            bool succeeded = DirectoryCreator::removeDirectory("non_empty_directory", true);

            THEN("the removal succeeds")
            {
                CHECK(succeeded);
            }

            THEN("the directory is actually gone")
            {
                CHECK_FALSE(DirectoryCreator::directoryExists("non_empty_directory"));
            }
        }

        WHEN("a non-existing directory is removed by force")
        {
            bool succeeded = DirectoryCreator::removeDirectory("magic_directory", true);

            THEN("the removal fails")
            {
                CHECK_FALSE(succeeded);
            }
        }

        //cleanup
        DirectoryCreator::removeDirectory("non_empty_directory", true);
    }
}
