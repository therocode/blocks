#include "imagefromfileloader.hpp"
#include "../utilities/lodepng.hpp"
#include "resourceexception.hpp"

Image ImageFromFileLoader::load(const std::string& filename)
{
    uint32_t width;
    uint32_t height;

    std::vector<unsigned char> image;

    unsigned error = lodepng::decode(image, width, height, filename);

    if(error)
    {
        throw ResourceException("Could not decode image " + filename + ". Error: " + std::string(lodepng_error_text(error)));
    }

    Image result({width, height}, image);

    return result;
}
