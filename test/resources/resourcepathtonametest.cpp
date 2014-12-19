#include "../catch.hpp"
#include "../../src/resources/resourcepathtoname.hpp"

SCENARIO("Resource paths can be converted to proper resource names", "[resources]")
{
    GIVEN("An asset folder with a couple of asset paths")
    {
        std::string assetFolder = "storage/game_stuff/assets";

        std::string image1 = "storage/game_stuff/assets/cool/graphics/images/yellowCactus.png";
        std::string image2 = "storage/game_stuff/assets/epic/graphics/images/brownGoat.png";
        std::string vertexShader1 = "storage/game_stuff/assets/cool/graphics/shaders/basic.vert";
        std::string vertexShader2 = "storage/game_stuff/assets/epic/graphics/shaders/spherical.vert";
        std::string fragmentShader1 = "storage/game_stuff/assets/cool/graphics/shaders/basic.frag";
        std::string fragmentShader2 = "storage/game_stuff/assets/epic/graphics/shaders/spherical.frag";
        std::string shader1 = "storage/game_stuff/assets/cool/graphics/shaders/basic.shad";
        std::string shader2 = "storage/game_stuff/assets/epic/graphics/shaders/spherical.shad";
        std::string model1 = "storage/game_stuff/assets/cool/graphics/models/pallkars.iqm";
        std::string model2 = "storage/game_stuff/assets/epic/graphics/models/korv.iqm";

        WHEN("The asset paths are turned into names")
        {
            THEN("The names are as expected")
            {
                CHECK(resourcePathToName("storage/game_stuff/assets", "storage/game_stuff/assets/cool/graphics/images/yellowCactus.png") == "cool.textures.yellowCactus" );
                CHECK(resourcePathToName("storage/game_stuff/assets", "storage/game_stuff/assets/epic/graphics/images/brownGoat.png") == "epic.textures.brownGoat" );
                CHECK(resourcePathToName("storage/game_stuff/assets", "storage/game_stuff/assets/cool/graphics/shaders/basic.vert") == "cool.vertex_shaders.basic" );
                CHECK(resourcePathToName("storage/game_stuff/assets", "storage/game_stuff/assets/epic/graphics/shaders/spherical.vert") == "epic.vertex_shaders.spherical" );
                CHECK(resourcePathToName("storage/game_stuff/assets", "storage/game_stuff/assets/cool/graphics/shaders/basic.frag") == "cool.fragment_shaders.basic" );
                CHECK(resourcePathToName("storage/game_stuff/assets", "storage/game_stuff/assets/epic/graphics/shaders/spherical.frag") == "epic.fragment_shaders.spherical" );
                CHECK(resourcePathToName("storage/game_stuff/assets", "storage/game_stuff/assets/cool/graphics/shaders/basic.shad") == "cool.shaders.basic" );
                CHECK(resourcePathToName("storage/game_stuff/assets", "storage/game_stuff/assets/epic/graphics/shaders/spherical.shad") == "epic.shaders.spherical" );
                CHECK(resourcePathToName("storage/game_stuff/assets", "storage/game_stuff/assets/cool/graphics/models/pallkars.iqm") == "cool.models.pallkars" );
                CHECK(resourcePathToName("storage/game_stuff/assets", "storage/game_stuff/assets/epic/graphics/models/korv.iqm") == "epic.models.korv" );
            }
        }
    }
}
