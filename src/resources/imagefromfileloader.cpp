#include "imagefromfileloader.hpp"
#include "../utilities/lodepng.hpp"
#include <iostream>

Image ImageFromFileLoader::load(const std::string& filename)
{
    uint32_t width;
    uint32_t height;

    std::vector<unsigned char> image;

    unsigned error = lodepng::decode(image, width, height, filename);

    if(error)
    {
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
        exit(0); //should be exception
    }

    Image result({width, height}, image);

    return result;
}
