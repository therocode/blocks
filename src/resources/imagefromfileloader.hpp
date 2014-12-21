#pragma once
#include <string>
#include "image.hpp"

class ImageFromFileLoader
{
    public:
        Image load(const std::string& filename);
    private:
};
